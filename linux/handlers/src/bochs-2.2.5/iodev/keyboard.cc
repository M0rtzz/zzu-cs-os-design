/////////////////////////////////////////////////////////////////////////
// $Id: keyboard.cc,v 1.106 2005/12/02 17:27:19 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


// Now features proper implementation of keyboard opcodes 0xF4 to 0xF6
// Silently ignores PS/2 keyboard extensions (0xF7 to 0xFD)
// Explicit panic on resend (0xFE)
//
// Emmanuel Marty <core@ggi-project.org>

// NB: now the PS/2 mouse support is in, outb changes meaning
// in conjunction with auxb
// auxb == 0 && outb == 0  => both buffers empty (nothing to read)
// auxb == 0 && outb == 1  => keyboard controller output buffer full
// auxb == 1 && outb == 0  => not used
// auxb == 1 && outb == 1  => mouse output buffer full.
// (das)

// Notes from Christophe Bothamy <cbbochs@free.fr>
//
// This file includes code from Ludovic Lange (http://ludovic.lange.free.fr)
// Implementation of 3 scancodes sets mf1,mf2,mf3 with or without translation. 
// Default is mf2 with translation
// Ability to switch between scancodes sets
// Ability to turn translation on or off

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include <math.h>
#include "scancodes.h"

#define LOG_THIS  theKeyboard->
#define VERBOSE_KBD_DEBUG 0


bx_keyb_c *theKeyboard = NULL;

  int
libkeyboard_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  // Create one instance of the keyboard device object.
  theKeyboard = new bx_keyb_c ();
  // Before this plugin was loaded, pluginKeyboard pointed to a stub.
  // Now make it point to the real thing.
  bx_devices.pluginKeyboard = theKeyboard;
  // Register this device.
  BX_REGISTER_DEVICE_DEVMODEL (plugin, type, theKeyboard, BX_PLUGIN_KEYBOARD);
  return(0); // Success
}

  void
libkeyboard_LTX_plugin_fini(void)
{
  BX_INFO (("keyboard plugin_fini"));
}

bx_keyb_c::bx_keyb_c(void)
{
  // constructor
  put("KBD");
  settype(KBDLOG);
}

bx_keyb_c::~bx_keyb_c(void)
{
  // destructor
  BX_DEBUG(("Exit."));
}


// flush internal buffer and reset keyboard settings to power-up condition
  void
bx_keyb_c::resetinternals(bx_bool powerup)
{
  Bit32u   i;

  BX_KEY_THIS s.kbd_internal_buffer.num_elements = 0;
  for (i=0; i<BX_KBD_ELEMENTS; i++)
    BX_KEY_THIS s.kbd_internal_buffer.buffer[i] = 0;
  BX_KEY_THIS s.kbd_internal_buffer.head = 0;

  BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic = 0;

  // Default scancode set is mf2 with translation
  BX_KEY_THIS s.kbd_controller.expecting_scancodes_set = 0;
  BX_KEY_THIS s.kbd_controller.current_scancodes_set = 1;
  BX_KEY_THIS s.kbd_controller.scancodes_translate = 1;
  
  if (powerup) {
    BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write = 0;
    BX_KEY_THIS s.kbd_internal_buffer.delay = 1; // 500 mS
    BX_KEY_THIS s.kbd_internal_buffer.repeat_rate = 0x0b; // 10.9 chars/sec
  }
}



  void
bx_keyb_c::init(void)
{
  BX_DEBUG(("Init $Id: keyboard.cc,v 1.106 2005/12/02 17:27:19 vruppert Exp $"));
  Bit32u   i;

  DEV_register_irq(1, "8042 Keyboard controller");
  DEV_register_irq(12, "8042 Keyboard controller (PS/2 mouse)");

  DEV_register_ioread_handler(this, read_handler,
                                      0x0060, "8042 Keyboard controller", 1);
  DEV_register_ioread_handler(this, read_handler,
                                      0x0064, "8042 Keyboard controller", 1);
  DEV_register_iowrite_handler(this, write_handler,
                                      0x0060, "8042 Keyboard controller", 1);
  DEV_register_iowrite_handler(this, write_handler,
                                      0x0064, "8042 Keyboard controller", 1);
  BX_KEY_THIS timer_handle = bx_pc_system.register_timer( this, timer_handler,
                                 bx_options.Okeyboard_serial_delay->get(), 1, 1,
				 "8042 Keyboard controller");

  resetinternals(1);

  BX_KEY_THIS s.kbd_internal_buffer.led_status = 0;
  BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 1;

  BX_KEY_THIS s.mouse_internal_buffer.num_elements = 0;
  for (i=0; i<BX_MOUSE_BUFF_SIZE; i++)
    BX_KEY_THIS s.mouse_internal_buffer.buffer[i] = 0;
  BX_KEY_THIS s.mouse_internal_buffer.head = 0;

  BX_KEY_THIS s.kbd_controller.pare = 0;
  BX_KEY_THIS s.kbd_controller.tim  = 0;
  BX_KEY_THIS s.kbd_controller.auxb = 0;
  BX_KEY_THIS s.kbd_controller.keyl = 1;
  BX_KEY_THIS s.kbd_controller.c_d  = 1;
  BX_KEY_THIS s.kbd_controller.sysf = 0;
  BX_KEY_THIS s.kbd_controller.inpb = 0;
  BX_KEY_THIS s.kbd_controller.outb = 0;

  BX_KEY_THIS s.kbd_controller.kbd_clock_enabled = 1;
  BX_KEY_THIS s.kbd_controller.aux_clock_enabled = 0;
  BX_KEY_THIS s.kbd_controller.allow_irq1 = 1;
  BX_KEY_THIS s.kbd_controller.allow_irq12 = 1;
  BX_KEY_THIS s.kbd_controller.kbd_output_buffer = 0;
  BX_KEY_THIS s.kbd_controller.aux_output_buffer = 0;
  BX_KEY_THIS s.kbd_controller.last_comm = 0;
  BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;
  BX_KEY_THIS s.kbd_controller.irq1_requested = 0;
  BX_KEY_THIS s.kbd_controller.irq12_requested = 0;
  BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 0;
  BX_KEY_THIS s.kbd_controller.bat_in_progress = 0;

  BX_KEY_THIS s.kbd_controller.timer_pending = 0;

  // Mouse initialization stuff
  BX_KEY_THIS s.mouse.type            = bx_options.Omouse_type->get();
  BX_KEY_THIS s.mouse.sample_rate     = 100; // reports per second
  BX_KEY_THIS s.mouse.resolution_cpmm = 4;   // 4 counts per millimeter
  BX_KEY_THIS s.mouse.scaling         = 1;   /* 1:1 (default) */
  BX_KEY_THIS s.mouse.mode            = MOUSE_MODE_RESET;
  BX_KEY_THIS s.mouse.enable          = 0;
  BX_KEY_THIS s.mouse.delayed_dx      = 0;
  BX_KEY_THIS s.mouse.delayed_dy      = 0;
  BX_KEY_THIS s.mouse.delayed_dz      = 0;
  BX_KEY_THIS s.mouse.im_request      = 0; // wheel mouse mode request
  BX_KEY_THIS s.mouse.im_mode         = 0; // wheel mouse mode

  for (i=0; i<BX_KBD_CONTROLLER_QSIZE; i++)
    BX_KEY_THIS s.controller_Q[i] = 0;
  BX_KEY_THIS s.controller_Qsize = 0;
  BX_KEY_THIS s.controller_Qsource = 0;

  // clear paste buffer
  BX_KEY_THIS pastebuf = NULL;
  BX_KEY_THIS pastebuf_len = 0;
  BX_KEY_THIS pastebuf_ptr = 0;
  BX_KEY_THIS paste_delay_changed(bx_options.Okeyboard_paste_delay->get());
  BX_KEY_THIS stop_paste = 0;

  // mouse port installed on system board
  DEV_cmos_set_reg(0x14, DEV_cmos_get_reg(0x14) | 0x04);

  // add keyboard LEDs to the statusbar
  BX_KEY_THIS statusbar_id[0] = bx_gui->register_statusitem("NUM");
  BX_KEY_THIS statusbar_id[1] = bx_gui->register_statusitem("CAPS");
  BX_KEY_THIS statusbar_id[2] = bx_gui->register_statusitem("SCRL");

#if BX_WITH_WX
  static bx_bool first_time = 1;
  if (first_time) {
    first_time = 0;
    // register shadow params (Experimental, not a complete list by far)
    bx_list_c *list = new bx_list_c (BXP_KBD_PARAMETERS, "Keyboard State", "", 20);
    list->add (new bx_shadow_bool_c (BXP_KBD_IRQ1_REQ, 
	  "Keyboard IRQ1 requested: ", "",
	  &BX_KEY_THIS s.kbd_controller.irq1_requested));
    list->add (new bx_shadow_bool_c (BXP_KBD_IRQ12_REQ,
	  "Keyboard IRQ12 requested: ", "",
	  &BX_KEY_THIS s.kbd_controller.irq12_requested));
    list->add (new bx_shadow_num_c (BXP_KBD_TIMER_PENDING,
	"Keyboard timer pending: ", "",
	&BX_KEY_THIS s.kbd_controller.timer_pending));
    list->add (new bx_shadow_bool_c (BXP_KBD_PARE,
	"Keyboard PARE", "",
	&BX_KEY_THIS s.kbd_controller.pare));
    list->add (new bx_shadow_bool_c (BXP_KBD_TIM,
	"Keyboard TIM", "",
	&BX_KEY_THIS s.kbd_controller.tim));
    list->add (new bx_shadow_bool_c (BXP_KBD_AUXB,
	"Keyboard AUXB", "",
	&BX_KEY_THIS s.kbd_controller.auxb));
    list->add (new bx_shadow_bool_c (BXP_KBD_KEYL,
	"Keyboard KEYL", "",
	&BX_KEY_THIS s.kbd_controller.keyl));
    list->add (new bx_shadow_bool_c (BXP_KBD_C_D,
	"Keyboard C_D", "",
	&BX_KEY_THIS s.kbd_controller.c_d));
    list->add (new bx_shadow_bool_c (BXP_KBD_SYSF,
	"Keyboard SYSF", "",
	&BX_KEY_THIS s.kbd_controller.sysf));
    list->add (new bx_shadow_bool_c (BXP_KBD_INPB,
	"Keyboard INPB", "",
	&BX_KEY_THIS s.kbd_controller.inpb));
    list->add (new bx_shadow_bool_c (BXP_KBD_OUTB,
	"Keyboard OUTB", "",
	&BX_KEY_THIS s.kbd_controller.outb));
  }
#endif

  // init runtime parameters
  bx_options.Omouse_enabled->set_handler(kbd_param_handler);
  bx_options.Omouse_enabled->set_runtime_param(1);
  bx_options.Okeyboard_paste_delay->set_handler(kbd_param_handler);
  bx_options.Okeyboard_paste_delay->set_runtime_param(1);
}

  void
bx_keyb_c::reset(unsigned type)
{
  if (BX_KEY_THIS pastebuf != NULL) {
    BX_KEY_THIS stop_paste = 1;
  }
}

Bit64s bx_keyb_c::kbd_param_handler(bx_param_c *param, int set, Bit64s val)
{
  if (set) {
    bx_id id = param->get_id ();
    switch (id) {
      case BXP_MOUSE_ENABLED:
        bx_gui->mouse_enabled_changed(val!=0);
        BX_KEY_THIS mouse_enabled_changed(val!=0);
        break;
      case BXP_KBD_PASTE_DELAY:
        BX_KEY_THIS paste_delay_changed(val);
        break;
      default:
        BX_PANIC(("kbd_param_handler called with unexpected parameter %d", id));
    }
  }
  return val;
}

  void
bx_keyb_c::paste_delay_changed(Bit32u value)
{
  BX_KEY_THIS pastedelay = value / BX_IODEV_HANDLER_PERIOD;
  BX_INFO(("will paste characters every %d keyboard ticks",BX_KEY_THIS pastedelay));
}

  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

// read function - the big picture:
// if address == data port then
//    if byte for mouse then return it
//    else if byte for keyboard then return it
// else address== status port
//    assemble the status bits and return them.
//
  Bit32u
bx_keyb_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_KEY_SMF
  bx_keyb_c *class_ptr = (bx_keyb_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_keyb_c::read(Bit32u   address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_KEY_SMF
  Bit8u val;

  if (address == 0x60) { /* output buffer */
    if (BX_KEY_THIS s.kbd_controller.auxb) { /* mouse byte available */
      val = BX_KEY_THIS s.kbd_controller.aux_output_buffer;
      BX_KEY_THIS s.kbd_controller.aux_output_buffer = 0;
      BX_KEY_THIS s.kbd_controller.outb = 0;
      BX_KEY_THIS s.kbd_controller.auxb = 0;
      BX_KEY_THIS s.kbd_controller.irq12_requested = 0;

      if (BX_KEY_THIS s.controller_Qsize) {
        unsigned i;
        BX_KEY_THIS s.kbd_controller.aux_output_buffer = BX_KEY_THIS s.controller_Q[0];
        BX_KEY_THIS s.kbd_controller.outb = 1;
        BX_KEY_THIS s.kbd_controller.auxb = 1;
        if (BX_KEY_THIS s.kbd_controller.allow_irq12)
          BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
        for (i=0; i<BX_KEY_THIS s.controller_Qsize-1; i++) {
          // move Q elements towards head of queue by one
          BX_KEY_THIS s.controller_Q[i] = BX_KEY_THIS s.controller_Q[i+1];
          }
        BX_KEY_THIS s.controller_Qsize--;
        }

      DEV_pic_lower_irq(12);
      activate_timer();
      BX_DEBUG(("[mouse] read from 0x%02x returns 0x%02x", address, val));
      return val;
      }
    else if (BX_KEY_THIS s.kbd_controller.outb) { /* kbd byte available */
      val = BX_KEY_THIS s.kbd_controller.kbd_output_buffer;
      BX_KEY_THIS s.kbd_controller.outb = 0;
      BX_KEY_THIS s.kbd_controller.auxb = 0;
      BX_KEY_THIS s.kbd_controller.irq1_requested = 0;
      BX_KEY_THIS s.kbd_controller.bat_in_progress = 0;

      if (BX_KEY_THIS s.controller_Qsize) {
        unsigned i;
        BX_KEY_THIS s.kbd_controller.aux_output_buffer = BX_KEY_THIS s.controller_Q[0];
        BX_KEY_THIS s.kbd_controller.outb = 1;
        BX_KEY_THIS s.kbd_controller.auxb = 1;
        if (BX_KEY_THIS s.kbd_controller.allow_irq1)
          BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
        for (i=0; i<BX_KEY_THIS s.controller_Qsize-1; i++) {
          // move Q elements towards head of queue by one
          BX_KEY_THIS s.controller_Q[i] = BX_KEY_THIS s.controller_Q[i+1];
          }
	BX_DEBUG(("s.controller_Qsize: %02X",BX_KEY_THIS s.controller_Qsize));
        BX_KEY_THIS s.controller_Qsize--;
        }

      DEV_pic_lower_irq(1);
      activate_timer();
      BX_DEBUG(("READ(%02x) = %02x", (unsigned) address,
          (unsigned) val));
      return val;
      }
    else {
      BX_DEBUG(("num_elements = %d", BX_KEY_THIS s.kbd_internal_buffer.num_elements));
      BX_DEBUG(("read from port 60h with outb empty"));
      return BX_KEY_THIS s.kbd_controller.kbd_output_buffer;
      }
    }

#if BX_CPU_LEVEL >= 2
  else if (address == 0x64) { /* status register */

    val = (BX_KEY_THIS s.kbd_controller.pare << 7)  |
          (BX_KEY_THIS s.kbd_controller.tim  << 6)  |
          (BX_KEY_THIS s.kbd_controller.auxb << 5)  |
          (BX_KEY_THIS s.kbd_controller.keyl << 4)  |
          (BX_KEY_THIS s.kbd_controller.c_d  << 3)  |
          (BX_KEY_THIS s.kbd_controller.sysf << 2)  |
          (BX_KEY_THIS s.kbd_controller.inpb << 1)  |
          BX_KEY_THIS s.kbd_controller.outb;
    BX_KEY_THIS s.kbd_controller.tim = 0;
    return val;
    }

#else /* BX_CPU_LEVEL > 0 */
  /* XT MODE, System 8255 Mode Register */
  else if (address == 0x64) { /* status register */
    BX_DEBUG(("IO read from port 64h, system 8255 mode register"));
    return BX_KEY_THIS s.kbd_controller.outb;
    }
#endif /* BX_CPU_LEVEL > 0 */

  BX_PANIC(("unknown address in io read to keyboard port %x",
      (unsigned) address));
  return 0; /* keep compiler happy */
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_keyb_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_KEY_SMF
  bx_keyb_c *class_ptr = (bx_keyb_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_keyb_c::write( Bit32u   address, Bit32u   value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_KEY_SMF
  Bit8u   command_byte;
  static int kbd_initialized=0;

  BX_DEBUG(("keyboard: 8-bit write to %04x = %02x", (unsigned)address, (unsigned)value));

  switch (address) {
    case 0x60: // input buffer
      // if expecting data byte from command last sent to port 64h
      if (BX_KEY_THIS s.kbd_controller.expecting_port60h) {
        BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;
        // data byte written last to 0x60
        BX_KEY_THIS s.kbd_controller.c_d = 0;
        if (BX_KEY_THIS s.kbd_controller.inpb) {
          BX_PANIC(("write to port 60h, not ready for write"));
          }
        switch (BX_KEY_THIS s.kbd_controller.last_comm) {
          case 0x60: // write command byte
            {
            bx_bool scan_convert, disable_keyboard,
                    disable_aux;

            scan_convert = (value >> 6) & 0x01;
            disable_aux      = (value >> 5) & 0x01;
            disable_keyboard = (value >> 4) & 0x01;
            BX_KEY_THIS s.kbd_controller.sysf = (value >> 2) & 0x01;
            BX_KEY_THIS s.kbd_controller.allow_irq1  = (value >> 0) & 0x01;
            BX_KEY_THIS s.kbd_controller.allow_irq12 = (value >> 1) & 0x01;
            set_kbd_clock_enable(!disable_keyboard);
            set_aux_clock_enable(!disable_aux);
            if (BX_KEY_THIS s.kbd_controller.allow_irq12 && BX_KEY_THIS s.kbd_controller.auxb)
              BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
            else if (BX_KEY_THIS s.kbd_controller.allow_irq1  && BX_KEY_THIS s.kbd_controller.outb)
              BX_KEY_THIS s.kbd_controller.irq1_requested = 1;

            BX_DEBUG(( " allow_irq12 set to %u",
              (unsigned) BX_KEY_THIS s.kbd_controller.allow_irq12));
            if ( !scan_convert )
              BX_ERROR(("keyboard: (mch) scan convert turned off"));

	    // (mch) NT needs this
	    BX_KEY_THIS s.kbd_controller.scancodes_translate = scan_convert;
            }
            break;
          case 0xd1: // write output port
            BX_DEBUG(("write output port with value %02xh", (unsigned) value));
	    BX_DEBUG(("write output port : %sable A20",(value & 0x02)?"en":"dis"));
            BX_SET_ENABLE_A20( (value & 0x02) != 0 );
            if (!(value & 0x01)) {
              BX_INFO(("write output port : processor reset requested!"));
              bx_pc_system.Reset( BX_RESET_SOFTWARE);
            }
            break;
          case 0xd4: // Write to mouse
            // I don't think this enables the AUX clock
            //set_aux_clock_enable(1); // enable aux clock line
            kbd_ctrl_to_mouse(value);
            // ??? should I reset to previous value of aux enable?
            break;

          case 0xd3: // write mouse output buffer
            // Queue in mouse output buffer
            controller_enQ(value, 1);
            break;

	  case 0xd2:
	    // Queue in keyboard output buffer
	    controller_enQ(value, 0);
	    break;

          default:
            BX_PANIC(("=== unsupported write to port 60h(lastcomm=%02x): %02x",
              (unsigned) BX_KEY_THIS s.kbd_controller.last_comm, (unsigned) value));
          }
      } else {
        // data byte written last to 0x60
        BX_KEY_THIS s.kbd_controller.c_d = 0;
        BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;
        /* pass byte to keyboard */
        /* ??? should conditionally pass to mouse device here ??? */
        if (BX_KEY_THIS s.kbd_controller.kbd_clock_enabled==0) {
          BX_ERROR(("keyboard disabled & send of byte %02x to kbd",
            (unsigned) value));
        }
        kbd_ctrl_to_kbd(value);
      }
      break;

    case 0x64: // control register
      // command byte written last to 0x64
      BX_KEY_THIS s.kbd_controller.c_d = 1;
      BX_KEY_THIS s.kbd_controller.last_comm = value;
      // most commands NOT expecting port60 write next
      BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;

      switch (value) {
        case 0x20: // get keyboard command byte
          BX_DEBUG(("get keyboard command byte"));
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
            BX_ERROR(("kbd: OUTB set and command 0x%02x encountered", value));
            break;
          }
          command_byte =
            (BX_KEY_THIS s.kbd_controller.scancodes_translate << 6) |
            ((!BX_KEY_THIS s.kbd_controller.aux_clock_enabled) << 5) |
            ((!BX_KEY_THIS s.kbd_controller.kbd_clock_enabled) << 4) |
            (0 << 3) |
            (BX_KEY_THIS s.kbd_controller.sysf << 2) |
            (BX_KEY_THIS s.kbd_controller.allow_irq12 << 1) |
            (BX_KEY_THIS s.kbd_controller.allow_irq1  << 0);
          controller_enQ(command_byte, 0);
          break;
        case 0x60: // write command byte
          BX_DEBUG(("write command byte"));
          // following byte written to port 60h is command byte
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xa0:
          BX_DEBUG(("keyboard BIOS name not supported"));
          break;

        case 0xa1:
          BX_DEBUG(("keyboard BIOS version not supported"));
          break;

        case 0xa7: // disable the aux device
          set_aux_clock_enable(0);
          BX_DEBUG(("aux device disabled"));
          break;
        case 0xa8: // enable the aux device
          set_aux_clock_enable(1);
          BX_DEBUG(("aux device enabled"));
          break;
        case 0xa9: // Test Mouse Port
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
            BX_PANIC(("kbd: OUTB set and command 0x%02x encountered", value));
            break;
          }
          controller_enQ(0x00, 0); // no errors detected
          break;
        case 0xaa: // motherboard controller self test
          BX_DEBUG(("Self Test"));
	  if (kbd_initialized == 0) {
	    BX_KEY_THIS s.controller_Qsize = 0;
	    BX_KEY_THIS s.kbd_controller.outb = 0;
	    kbd_initialized++;
	  }
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
            BX_ERROR(("kbd: OUTB set and command 0x%02x encountered", value));
            break;
          }
	  // (mch) Why is this commented out??? Enabling
          BX_KEY_THIS s.kbd_controller.sysf = 1; // self test complete
          controller_enQ(0x55, 0); // controller OK
          break;
        case 0xab: // Interface Test
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
            BX_PANIC(("kbd: OUTB set and command 0x%02x encountered", value));
            break;
          }
          controller_enQ(0x00, 0);
          break;
        case 0xad: // disable keyboard
          set_kbd_clock_enable(0);
          BX_DEBUG(("keyboard disabled"));
          break;
        case 0xae: // enable keyboard
          set_kbd_clock_enable(1);
          BX_DEBUG(("keyboard enabled"));
          break;
        case 0xaf: // get controller version
          BX_INFO(("'get controller version' not supported yet"));
          break;
        case 0xc0: // read input port
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
            BX_PANIC(("kbd: OUTB set and command 0x%02x encountered", value));
            break;
          }
          // keyboard not inhibited
          controller_enQ(0x80, 0);
          break;
        case 0xd0: // read output port: next byte read from port 60h
          BX_DEBUG(("io write to port 64h, command d0h (partial)"));
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
            BX_PANIC(("kbd: OUTB set and command 0x%02x encountered", value));
            break;
          }
          controller_enQ(
              (BX_KEY_THIS s.kbd_controller.irq12_requested << 5) |
              (BX_KEY_THIS s.kbd_controller.irq1_requested << 4) |
              (BX_GET_ENABLE_A20() << 1) |
              0x01, 0);
          break;

        case 0xd1: // write output port: next byte written to port 60h
          BX_DEBUG(("write output port"));
          // following byte to port 60h written to output port
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xd3: // write mouse output buffer
	  //FIXME: Why was this a panic?
          BX_DEBUG(("io write 0x64: command = 0xD3(write mouse outb)"));
	  // following byte to port 60h written to output port as mouse write.
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xd4: // write to mouse
          BX_DEBUG(("io write 0x64: command = 0xD4 (write to mouse)"));
          // following byte written to port 60h
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xd2: // write keyboard output buffer
	  BX_DEBUG(("io write 0x64: write keyboard output buffer"));
	  BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
	  break;
        case 0xdd: // Disable A20 Address Line
	  BX_SET_ENABLE_A20(0);
	  break;
        case 0xdf: // Enable A20 Address Line
	  BX_SET_ENABLE_A20(1);
	  break;
        case 0xc1: // Continuous Input Port Poll, Low
        case 0xc2: // Continuous Input Port Poll, High
        case 0xe0: // Read Test Inputs
          BX_PANIC(("io write 0x64: command = %02xh", (unsigned) value));
          break;

        case 0xfe: // System (cpu?) Reset, transition to real mode
          BX_INFO(("io write 0x64: command 0xfe: reset cpu"));
          bx_pc_system.Reset( BX_RESET_SOFTWARE );
          break;

        default:
          if (value==0xff || (value>=0xf0 && value<=0xfd)) {
            /* useless pulse output bit commands ??? */
            BX_DEBUG(("io write to port 64h, useless command %02x",
                (unsigned) value));
            return;
          }
          BX_PANIC(("unsupported io write to keyboard port %x, value = %x",
            (unsigned) address, (unsigned) value));
          break;
        }
      break;

    default: BX_PANIC(("unknown address in bx_keyb_c::write()"));
  }
}

// service_paste_buf() transfers data from the paste buffer to the hardware
// keyboard buffer.  It tries to transfer as many chars as possible at a
// time, but because different chars require different numbers of scancodes
// we have to be conservative.  Note that this process depends on the
// keymap tables to know what chars correspond to what keys, and which
// chars require a shift or other modifier.
void 
bx_keyb_c::service_paste_buf ()
{
  if (!BX_KEY_THIS pastebuf) return;
  BX_DEBUG (("service_paste_buf: ptr at %d out of %d", BX_KEY_THIS pastebuf_ptr, BX_KEY_THIS pastebuf_len));
  int fill_threshold = BX_KBD_ELEMENTS - 8;
  while ( (BX_KEY_THIS pastebuf_ptr < BX_KEY_THIS pastebuf_len) && ! BX_KEY_THIS stop_paste) {
    if (BX_KEY_THIS s.kbd_internal_buffer.num_elements >= fill_threshold)
      return;
    // there room in the buffer for a keypress and a key release.
    // send one keypress and a key release.
    Bit8u byte = BX_KEY_THIS pastebuf[BX_KEY_THIS pastebuf_ptr];
    BXKeyEntry *entry = bx_keymap.findAsciiChar (byte);
    if (!entry) {
      BX_ERROR (("paste character 0x%02x ignored", byte));
    } else {
      BX_DEBUG (("pasting character 0x%02x. baseKey is %04x", byte, entry->baseKey));
      if (entry->modKey != BX_KEYMAP_UNKNOWN)
        BX_KEY_THIS gen_scancode (entry->modKey);
      BX_KEY_THIS gen_scancode (entry->baseKey);
      BX_KEY_THIS gen_scancode (entry->baseKey | BX_KEY_RELEASED);
      if (entry->modKey != BX_KEYMAP_UNKNOWN)
        BX_KEY_THIS gen_scancode (entry->modKey | BX_KEY_RELEASED);
    }
    BX_KEY_THIS pastebuf_ptr++;
  }
  // reached end of pastebuf.  free the memory it was using.
  delete [] BX_KEY_THIS pastebuf;
  BX_KEY_THIS pastebuf = NULL;
  BX_KEY_THIS stop_paste = 0;
}

// paste_bytes schedules an arbitrary number of ASCII characters to be
// inserted into the hardware queue as it become available.  Any previous
// paste which is still in progress will be thrown out.  BYTES is a pointer
// to a region of memory containing the chars to be pasted. When the paste
// is complete, the keyboard code will call delete [] bytes;
void
bx_keyb_c::paste_bytes (Bit8u *bytes, Bit32s length)
{
  BX_DEBUG (("paste_bytes: %d bytes", length));
  if (BX_KEY_THIS pastebuf) {
    BX_ERROR (("previous paste was not completed!  %d chars lost", 
      BX_KEY_THIS pastebuf_len - BX_KEY_THIS pastebuf_ptr));
    delete [] BX_KEY_THIS pastebuf;  // free the old paste buffer
  }
  BX_KEY_THIS pastebuf = bytes;
  BX_KEY_THIS pastebuf_ptr = 0;
  BX_KEY_THIS pastebuf_len = length;
  BX_KEY_THIS service_paste_buf ();
}

  void
bx_keyb_c::gen_scancode(Bit32u key)
{
  unsigned char *scancode;
  Bit8u  i;

  BX_DEBUG(( "gen_scancode(): %s %s", bx_keymap.getBXKeyName(key), (key >> 31)?"released":"pressed"));

  if (!BX_KEY_THIS s.kbd_controller.scancodes_translate)
	BX_DEBUG(("keyboard: gen_scancode with scancode_translate cleared"));

  // Ignore scancode if keyboard clock is driven low
  if (BX_KEY_THIS s.kbd_controller.kbd_clock_enabled==0)
    return;

  // Ignore scancode if scanning is disabled
  if (BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled==0)
    return;

  // Switch between make and break code
  if (key & BX_KEY_RELEASED)
    scancode=(unsigned char *)scancodes[(key&0xFF)][BX_KEY_THIS s.kbd_controller.current_scancodes_set].brek;
  else
    scancode=(unsigned char *)scancodes[(key&0xFF)][BX_KEY_THIS s.kbd_controller.current_scancodes_set].make;

#if BX_SUPPORT_PCIUSB
  if (DEV_usb_keyboard_connected()) {
    // if we have a keyboard/keypad installed, we need to call its handler first
    if (DEV_usb_key_enq(scancode)) return;
  }
#endif


  if (BX_KEY_THIS s.kbd_controller.scancodes_translate) {
    // Translate before send
    Bit8u escaped=0x00;

    for (i=0; i<strlen( (const char *)scancode ); i++) {
      if (scancode[i] == 0xF0)
        escaped=0x80;
      else {
	BX_DEBUG(("gen_scancode(): writing translated %02x",translation8042[scancode[i] ] | escaped));
        kbd_enQ(translation8042[scancode[i] ] | escaped );
        escaped=0x00;
      }
    }
  } 
  else {
    // Send raw data
    for (i=0; i<strlen( (const char *)scancode ); i++) {
      BX_DEBUG(("gen_scancode(): writing raw %02x",scancode[i]));
      kbd_enQ( scancode[i] );
    }
  }
}



  void BX_CPP_AttrRegparmN(1)
bx_keyb_c::set_kbd_clock_enable(Bit8u   value)
{
  bx_bool prev_kbd_clock_enabled;

  if (value==0) {
    BX_KEY_THIS s.kbd_controller.kbd_clock_enabled = 0;
  } else {
    /* is another byte waiting to be sent from the keyboard ? */
    prev_kbd_clock_enabled = BX_KEY_THIS s.kbd_controller.kbd_clock_enabled;
    BX_KEY_THIS s.kbd_controller.kbd_clock_enabled = 1;

    if (prev_kbd_clock_enabled==0 && BX_KEY_THIS s.kbd_controller.outb==0) {
      activate_timer();
    }
  }
}



  void
bx_keyb_c::set_aux_clock_enable(Bit8u   value)
{
  bx_bool prev_aux_clock_enabled;

  BX_DEBUG(("set_aux_clock_enable(%u)", (unsigned) value));
  if (value==0) {
    BX_KEY_THIS s.kbd_controller.aux_clock_enabled = 0;
  } else {
    /* is another byte waiting to be sent from the keyboard ? */
    prev_aux_clock_enabled = BX_KEY_THIS s.kbd_controller.aux_clock_enabled;
    BX_KEY_THIS s.kbd_controller.aux_clock_enabled = 1;
    if (prev_aux_clock_enabled==0 && BX_KEY_THIS s.kbd_controller.outb==0)
      activate_timer();
  }
}

  Bit8u
bx_keyb_c::get_kbd_enable(void)
{
  BX_DEBUG(("get_kbd_enable(): getting kbd_clock_enabled of: %02x",
      (unsigned) BX_KEY_THIS s.kbd_controller.kbd_clock_enabled));

  return(BX_KEY_THIS s.kbd_controller.kbd_clock_enabled);
}

  void
bx_keyb_c::controller_enQ(Bit8u data, unsigned source)
{
  // source is 0 for keyboard, 1 for mouse

  BX_DEBUG(("controller_enQ(%02x) source=%02x", (unsigned) data,source));

  // see if we need to Q this byte from the controller
  // remember this includes mouse bytes.
  if (BX_KEY_THIS s.kbd_controller.outb) {
    if (BX_KEY_THIS s.controller_Qsize >= BX_KBD_CONTROLLER_QSIZE)
      BX_PANIC(("controller_enq(): controller_Q full!"));
    BX_KEY_THIS s.controller_Q[BX_KEY_THIS s.controller_Qsize++] = data;
    BX_KEY_THIS s.controller_Qsource = source;
    return;
  }

  // the Q is empty
  if (source == 0) { // keyboard
    BX_KEY_THIS s.kbd_controller.kbd_output_buffer = data;
    BX_KEY_THIS s.kbd_controller.outb = 1;
    BX_KEY_THIS s.kbd_controller.auxb = 0;
    BX_KEY_THIS s.kbd_controller.inpb = 0;
    if (BX_KEY_THIS s.kbd_controller.allow_irq1)
      BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
  } else { // mouse
    BX_KEY_THIS s.kbd_controller.aux_output_buffer = data;
    BX_KEY_THIS s.kbd_controller.outb = 1;
    BX_KEY_THIS s.kbd_controller.auxb = 1;
    BX_KEY_THIS s.kbd_controller.inpb = 0;
    if (BX_KEY_THIS s.kbd_controller.allow_irq12)
      BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
  }
}

void
bx_keyb_c::kbd_enQ_imm(Bit8u val)
{
  int tail;

  if (BX_KEY_THIS s.kbd_internal_buffer.num_elements >= BX_KBD_ELEMENTS) {
    BX_PANIC(("internal keyboard buffer full (imm)"));
    return;
  }

  /* enqueue scancode in multibyte internal keyboard buffer */
  tail = (BX_KEY_THIS s.kbd_internal_buffer.head + BX_KEY_THIS s.kbd_internal_buffer.num_elements) %
    BX_KBD_ELEMENTS;

  BX_KEY_THIS s.kbd_controller.kbd_output_buffer = val;
  BX_KEY_THIS s.kbd_controller.outb = 1;

  if (BX_KEY_THIS s.kbd_controller.allow_irq1)
    BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
}


  void
bx_keyb_c::kbd_enQ(Bit8u scancode)
{
  int tail;

  BX_DEBUG(("kbd_enQ(0x%02x)", (unsigned) scancode));

  if (BX_KEY_THIS s.kbd_internal_buffer.num_elements >= BX_KBD_ELEMENTS) {
    BX_INFO(("internal keyboard buffer full, ignoring scancode.(%02x)",
      (unsigned) scancode));
    return;
  }

  /* enqueue scancode in multibyte internal keyboard buffer */
  BX_DEBUG(("kbd_enQ: putting scancode 0x%02x in internal buffer",
      (unsigned) scancode));
  tail = (BX_KEY_THIS s.kbd_internal_buffer.head + BX_KEY_THIS s.kbd_internal_buffer.num_elements) %
   BX_KBD_ELEMENTS;
  BX_KEY_THIS s.kbd_internal_buffer.buffer[tail] = scancode;
  BX_KEY_THIS s.kbd_internal_buffer.num_elements++;

  if (!BX_KEY_THIS s.kbd_controller.outb && BX_KEY_THIS s.kbd_controller.kbd_clock_enabled) {
    activate_timer();
    BX_DEBUG(("activating timer..."));
    return;
  }
}

  bx_bool
bx_keyb_c::mouse_enQ_packet(Bit8u b1, Bit8u b2, Bit8u b3, Bit8u b4)
{
  int bytes = 3;
  if (BX_KEY_THIS s.mouse.im_mode) bytes = 4;

  if ((BX_KEY_THIS s.mouse_internal_buffer.num_elements + bytes) >= BX_MOUSE_BUFF_SIZE) {
    return(0); /* buffer doesn't have the space */
  }

  mouse_enQ(b1);
  mouse_enQ(b2);
  mouse_enQ(b3);
  if (BX_KEY_THIS s.mouse.im_mode) mouse_enQ(b4);

  return(1);
}


  void
bx_keyb_c::mouse_enQ(Bit8u   mouse_data)
{
  int tail;

  BX_DEBUG(("mouse_enQ(%02x)", (unsigned) mouse_data));

  if (BX_KEY_THIS s.mouse_internal_buffer.num_elements >= BX_MOUSE_BUFF_SIZE) {
    BX_ERROR(("[mouse] internal mouse buffer full, ignoring mouse data.(%02x)",
      (unsigned) mouse_data));
    return;
  }

  /* enqueue mouse data in multibyte internal mouse buffer */
  tail = (BX_KEY_THIS s.mouse_internal_buffer.head + BX_KEY_THIS s.mouse_internal_buffer.num_elements) %
   BX_MOUSE_BUFF_SIZE;
  BX_KEY_THIS s.mouse_internal_buffer.buffer[tail] = mouse_data;
  BX_KEY_THIS s.mouse_internal_buffer.num_elements++;

  if (!BX_KEY_THIS s.kbd_controller.outb && BX_KEY_THIS s.kbd_controller.aux_clock_enabled) {
    activate_timer();
    return;
  }
}

  void
bx_keyb_c::kbd_ctrl_to_kbd(Bit8u   value)
{

  BX_DEBUG(("controller passed byte %02xh to keyboard", value));

  if (BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic) {
    BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic = 0;
    BX_KEY_THIS s.kbd_internal_buffer.delay = (value >> 5) & 0x03;
    switch (BX_KEY_THIS s.kbd_internal_buffer.delay) {
      case 0: BX_INFO(("setting delay to 250 mS (unused)")); break;
      case 1: BX_INFO(("setting delay to 500 mS (unused)")); break;
      case 2: BX_INFO(("setting delay to 750 mS (unused)")); break;
      case 3: BX_INFO(("setting delay to 1000 mS (unused)")); break;
    }
    BX_KEY_THIS s.kbd_internal_buffer.repeat_rate = value & 0x1f;
    double cps = 1 /((double)(8 + (value & 0x07)) * (double)exp(log((double)2) * (double)((value >> 3) & 0x03)) * 0.00417);
    BX_INFO(("setting repeat rate to %.1f cps (unused)", cps));
    kbd_enQ(0xFA); // send ACK
    return;
  }

  if (BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write) {
    BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write = 0;
    BX_KEY_THIS s.kbd_internal_buffer.led_status = value;
    BX_DEBUG(("LED status set to %02x",
      (unsigned) BX_KEY_THIS s.kbd_internal_buffer.led_status));
    bx_gui->statusbar_setitem(BX_KEY_THIS statusbar_id[0], value & 0x02);
    bx_gui->statusbar_setitem(BX_KEY_THIS statusbar_id[1], value & 0x04);
    bx_gui->statusbar_setitem(BX_KEY_THIS statusbar_id[2], value & 0x01);
    kbd_enQ(0xFA); // send ACK %%%
    return;
  }

  if (BX_KEY_THIS s.kbd_controller.expecting_scancodes_set) {
    BX_KEY_THIS s.kbd_controller.expecting_scancodes_set = 0;
    if( value != 0 ) {
      if( value<4 ) {
        BX_KEY_THIS s.kbd_controller.current_scancodes_set = (value-1);
        BX_INFO(("Switched to scancode set %d",
          (unsigned) BX_KEY_THIS s.kbd_controller.current_scancodes_set + 1));
        kbd_enQ(0xFA);
      } else {
        BX_ERROR(("Received scancodes set out of range: %d", value ));
        kbd_enQ(0xFF); // send ERROR
      }
    } else {
      // Send ACK (SF patch #1159626)
      kbd_enQ(0xFA);
      // Send current scancodes set to port 0x60
      kbd_enQ( 1 + (BX_KEY_THIS s.kbd_controller.current_scancodes_set) ); 
    }
    return;
  }

  switch (value) {
    case 0x00: // ??? ignore and let OS timeout with no response
      kbd_enQ(0xFA); // send ACK %%%
      break;

    case 0x05: // ???
      // (mch) trying to get this to work...
      BX_KEY_THIS s.kbd_controller.sysf = 1;
      kbd_enQ_imm(0xfe);
      break;

    case 0xed: // LED Write
      BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write = 1;
      kbd_enQ_imm(0xFA); // send ACK %%%
      break;

    case 0xee: // echo
      kbd_enQ(0xEE); // return same byte (EEh) as echo diagnostic
      break;

    case 0xf0: // Select alternate scan code set
      BX_KEY_THIS s.kbd_controller.expecting_scancodes_set = 1;
      BX_DEBUG(("Expecting scancode set info..."));
      kbd_enQ(0xFA); // send ACK
      break;

    case 0xf2:  // identify keyboard
      BX_INFO(("identify keyboard command received"));

      // XT sends nothing, AT sends ACK 
      // MFII with translation sends ACK+ABh+41h
      // MFII without translation sends ACK+ABh+83h
      if (bx_options.Okeyboard_type->get() != BX_KBD_XT_TYPE) {
        kbd_enQ(0xFA); 
        if (bx_options.Okeyboard_type->get() == BX_KBD_MF_TYPE) {
          kbd_enQ(0xAB);
          
          if(BX_KEY_THIS s.kbd_controller.scancodes_translate)
            kbd_enQ(0x41);
          else
            kbd_enQ(0x83);
        }
      }
      break;

    case 0xf3:  // typematic info
      BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic = 1;
      BX_INFO(("setting typematic info"));
      kbd_enQ(0xFA); // send ACK
      break;

    case 0xf4:  // enable keyboard
      BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 1;
      kbd_enQ(0xFA); // send ACK
      break;

    case 0xf5:  // reset keyboard to power-up settings and disable scanning
      resetinternals(1);
      kbd_enQ(0xFA); // send ACK
      BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 0;
      BX_INFO(("reset-disable command received"));
      break;

    case 0xf6:  // reset keyboard to power-up settings and enable scanning
      resetinternals(1);
      kbd_enQ(0xFA); // send ACK
      BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 1;
      BX_INFO(("reset-enable command received"));
      break;

    case 0xfe:  // resend. aiiee.
      BX_PANIC( ("got 0xFE (resend)"));
      break;

    case 0xff:  // reset: internal keyboard reset and afterwards the BAT
      BX_DEBUG(("reset command received"));
      resetinternals(1);
      kbd_enQ(0xFA); // send ACK
      BX_KEY_THIS s.kbd_controller.bat_in_progress = 1;
      kbd_enQ(0xAA); // BAT test passed
      break;

    case 0xd3:
      kbd_enQ(0xfa);
      break;

    case 0xf7:  // PS/2 Set All Keys To Typematic
    case 0xf8:  // PS/2 Set All Keys to Make/Break
    case 0xf9:  // PS/2 PS/2 Set All Keys to Make
    case 0xfa:  // PS/2 Set All Keys to Typematic Make/Break
    case 0xfb:  // PS/2 Set Key Type to Typematic
    case 0xfc:  // PS/2 Set Key Type to Make/Break
    case 0xfd:  // PS/2 Set Key Type to Make
    default:
      BX_ERROR(("kbd_ctrl_to_kbd(): got value of 0x%02x", value));
      kbd_enQ(0xFE); /* send NACK */
      break;
  }
}

  void
bx_keyb_c::timer_handler(void *this_ptr)
{
  bx_keyb_c *class_ptr = (bx_keyb_c *) this_ptr;
  unsigned retval;

  // retval=class_ptr->periodic( bx_options.Okeyboard_serial_delay->get());
  retval=class_ptr->periodic(1);

  if(retval&0x01)
    DEV_pic_raise_irq(1);
  if(retval&0x02)
    DEV_pic_raise_irq(12);
}

  unsigned
bx_keyb_c::periodic( Bit32u   usec_delta )
{
/*  static int multiple=0; */
  static unsigned count_before_paste=0;
  Bit8u   retval;

  UNUSED( usec_delta );

  if (BX_KEY_THIS s.kbd_controller.kbd_clock_enabled ) {
    if(++count_before_paste>=BX_KEY_THIS pastedelay) {
      // after the paste delay, consider adding moving more chars
      // from the paste buffer to the keyboard buffer.
      BX_KEY_THIS service_paste_buf ();
      count_before_paste=0;
    }
  }

  retval = BX_KEY_THIS s.kbd_controller.irq1_requested | (BX_KEY_THIS s.kbd_controller.irq12_requested << 1);
  BX_KEY_THIS s.kbd_controller.irq1_requested = 0;
  BX_KEY_THIS s.kbd_controller.irq12_requested = 0;

  if ( BX_KEY_THIS s.kbd_controller.timer_pending == 0 ) {
    return(retval);
  }

  if ( usec_delta >= BX_KEY_THIS s.kbd_controller.timer_pending ) {
    BX_KEY_THIS s.kbd_controller.timer_pending = 0;
  } else {
    BX_KEY_THIS s.kbd_controller.timer_pending -= usec_delta;
    return(retval);
  }

  if (BX_KEY_THIS s.kbd_controller.outb) {
    return(retval);
  }

  /* nothing in outb, look for possible data xfer from keyboard or mouse */
  if (BX_KEY_THIS s.kbd_internal_buffer.num_elements &&
      (BX_KEY_THIS s.kbd_controller.kbd_clock_enabled || BX_KEY_THIS s.kbd_controller.bat_in_progress)) {
    BX_DEBUG(("service_keyboard: key in internal buffer waiting"));
    BX_KEY_THIS s.kbd_controller.kbd_output_buffer =
      BX_KEY_THIS s.kbd_internal_buffer.buffer[BX_KEY_THIS s.kbd_internal_buffer.head];
    BX_KEY_THIS s.kbd_controller.outb = 1;
    // commented out since this would override the current state of the
    // mouse buffer flag - no bug seen - just seems wrong (das)
    //    BX_KEY_THIS s.kbd_controller.auxb = 0;
    BX_KEY_THIS s.kbd_internal_buffer.head = (BX_KEY_THIS s.kbd_internal_buffer.head + 1) %
      BX_KBD_ELEMENTS;
    BX_KEY_THIS s.kbd_internal_buffer.num_elements--;
    if (BX_KEY_THIS s.kbd_controller.allow_irq1)
      BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
  } else { 
    create_mouse_packet(0);
    if (BX_KEY_THIS s.kbd_controller.aux_clock_enabled && BX_KEY_THIS s.mouse_internal_buffer.num_elements) {
      BX_DEBUG(("service_keyboard: key(from mouse) in internal buffer waiting"));
      BX_KEY_THIS s.kbd_controller.aux_output_buffer =
	BX_KEY_THIS s.mouse_internal_buffer.buffer[BX_KEY_THIS s.mouse_internal_buffer.head];

      BX_KEY_THIS s.kbd_controller.outb = 1;
      BX_KEY_THIS s.kbd_controller.auxb = 1;
      BX_KEY_THIS s.mouse_internal_buffer.head = (BX_KEY_THIS s.mouse_internal_buffer.head + 1) %
	BX_MOUSE_BUFF_SIZE;
      BX_KEY_THIS s.mouse_internal_buffer.num_elements--;
      if (BX_KEY_THIS s.kbd_controller.allow_irq12)
	BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
    } else {
      BX_DEBUG(("service_keyboard(): no keys waiting"));
    }
  }
  return(retval);
}




  void
bx_keyb_c::activate_timer(void)
{
  if (BX_KEY_THIS s.kbd_controller.timer_pending == 0) {
    BX_KEY_THIS s.kbd_controller.timer_pending = 1;
  }
}


  void
bx_keyb_c::kbd_ctrl_to_mouse(Bit8u   value)
{
  // if we are not using a ps2 mouse, some of the following commands need to return different values
  bx_bool is_ps2 = 0;
  if ((bx_options.Omouse_type->get() == BX_MOUSE_TYPE_PS2) ||
      (bx_options.Omouse_type->get() == BX_MOUSE_TYPE_IMPS2)) is_ps2 = 1;

  BX_DEBUG(("MOUSE: kbd_ctrl_to_mouse(%02xh)", (unsigned) value));
  BX_DEBUG(("  enable = %u", (unsigned) BX_KEY_THIS s.mouse.enable));
  BX_DEBUG(("  allow_irq12 = %u",
    (unsigned) BX_KEY_THIS s.kbd_controller.allow_irq12));
  BX_DEBUG(("  aux_clock_enabled = %u",
    (unsigned) BX_KEY_THIS s.kbd_controller.aux_clock_enabled));

  // an ACK (0xFA) is always the first response to any valid input
  // received from the system other than Set-Wrap-Mode & Resend-Command

  if (BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter) {
    BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 0;
    switch (BX_KEY_THIS s.kbd_controller.last_mouse_command) {
      case 0xf3: // Set Mouse Sample Rate
        BX_KEY_THIS s.mouse.sample_rate = value;
        BX_DEBUG(("[mouse] Sampling rate set: %d Hz", value));
        if ((value == 200) && (!BX_KEY_THIS s.mouse.im_request)) {
          BX_KEY_THIS s.mouse.im_request = 1;
        } else if ((value == 100) && (BX_KEY_THIS s.mouse.im_request == 1)) {
          BX_KEY_THIS s.mouse.im_request = 2;
        } else if ((value == 80) && (BX_KEY_THIS s.mouse.im_request == 2)) {
          if (BX_KEY_THIS s.mouse.type == BX_MOUSE_TYPE_IMPS2) {
            BX_INFO(("wheel mouse mode enabled"));
            BX_KEY_THIS s.mouse.im_mode = 1;
          } else {
            BX_INFO(("wheel mouse mode request rejected"));
          }
          BX_KEY_THIS s.mouse.im_request = 0;
        } else {
          BX_KEY_THIS s.mouse.im_request = 0;
        }
        controller_enQ(0xFA, 1); // ack
        break;

      case 0xe8: // Set Mouse Resolution
        switch (value) {
          case 0:
            BX_KEY_THIS s.mouse.resolution_cpmm = 1;
            break;
          case 1:
            BX_KEY_THIS s.mouse.resolution_cpmm = 2;
            break;
          case 2:
            BX_KEY_THIS s.mouse.resolution_cpmm = 4;
            break;
          case 3:
            BX_KEY_THIS s.mouse.resolution_cpmm = 8;
            break;
          default:
            BX_PANIC(("[mouse] Unknown resolution %d", value));
            break;
        }
        BX_DEBUG(("[mouse] Resolution set to %d counts per mm",
          BX_KEY_THIS s.mouse.resolution_cpmm));

        controller_enQ(0xFA, 1); // ack
        break;

      default:
        BX_PANIC(("MOUSE: unknown last command (%02xh)", (unsigned) BX_KEY_THIS s.kbd_controller.last_mouse_command));
    }
  } else {
    BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 0;
    BX_KEY_THIS s.kbd_controller.last_mouse_command = value;

    // test for wrap mode first
    if (BX_KEY_THIS s.mouse.mode == MOUSE_MODE_WRAP) {
      // if not a reset command or reset wrap mode
      // then just echo the byte.
      if ((value != 0xff) && (value != 0xec)) {
        if (bx_dbg.mouse)
          BX_INFO(("[mouse] wrap mode: Ignoring command %0X02.",value));
        controller_enQ(value,1);
        // bail out
        return;
      }
    }
    switch ( value ) {
      case 0xe6: // Set Mouse Scaling to 1:1
        controller_enQ(0xFA, 1); // ACK
        BX_KEY_THIS s.mouse.scaling = 2;
        BX_DEBUG(("[mouse] Scaling set to 1:1"));
        break;

      case 0xe7: // Set Mouse Scaling to 2:1
        controller_enQ(0xFA, 1); // ACK
        BX_KEY_THIS s.mouse.scaling         = 2;
        BX_DEBUG(("[mouse] Scaling set to 2:1"));
        break;

      case 0xe8: // Set Mouse Resolution
        controller_enQ(0xFA, 1); // ACK
        BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 1;
        break;

      case 0xea: // Set Stream Mode
        if (bx_dbg.mouse)
          BX_INFO(("[mouse] Mouse stream mode on."));
        BX_KEY_THIS s.mouse.mode = MOUSE_MODE_STREAM;
        controller_enQ(0xFA, 1); // ACK
        break;

      case 0xec: // Reset Wrap Mode
        // unless we are in wrap mode ignore the command
        if ( BX_KEY_THIS s.mouse.mode == MOUSE_MODE_WRAP) {
          if (bx_dbg.mouse)
            BX_INFO(("[mouse] Mouse wrap mode off."));
          // restore previous mode except disable stream mode reporting.
          // ### TODO disabling reporting in stream mode
          BX_KEY_THIS s.mouse.mode = BX_KEY_THIS s.mouse.saved_mode;
          controller_enQ(0xFA, 1); // ACK
        }
        break;
      case 0xee: // Set Wrap Mode
        // ### TODO flush output queue.
        // ### TODO disable interrupts if in stream mode.
        if (bx_dbg.mouse)
          BX_INFO(("[mouse] Mouse wrap mode on."));
        BX_KEY_THIS s.mouse.saved_mode = BX_KEY_THIS s.mouse.mode;
        BX_KEY_THIS s.mouse.mode = MOUSE_MODE_WRAP;
        controller_enQ(0xFA, 1); // ACK
        break;

      case 0xf0: // Set Remote Mode (polling mode, i.e. not stream mode.)
        if (bx_dbg.mouse)
          BX_INFO(("[mouse] Mouse remote mode on."));
        // ### TODO should we flush/discard/ignore any already queued packets?
        BX_KEY_THIS s.mouse.mode = MOUSE_MODE_REMOTE;
        controller_enQ(0xFA, 1); // ACK
        break;

      case 0xf2: // Read Device Type
        controller_enQ(0xFA, 1); // ACK
        if (BX_KEY_THIS s.mouse.im_mode)
          controller_enQ(0x03, 1); // Device ID (wheel z-mouse)
        else
          controller_enQ(0x00, 1); // Device ID (standard)
        BX_DEBUG(("[mouse] Read mouse ID"));
        break;

      case 0xf3: // Set Mouse Sample Rate (sample rate written to port 60h)
        controller_enQ(0xFA, 1); // ACK
        BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 1;
        break;

      case 0xf4: // Enable (in stream mode)
        // is a mouse present?
        if (is_ps2) {
          BX_KEY_THIS s.mouse.enable = 1;
          controller_enQ(0xFA, 1); // ACK
          BX_DEBUG(("[mouse] Mouse enabled (stream mode)"));
        } else {
          // a mouse isn't present.  We need to return a 0xFE (resend) instead of a 0xFA (ACK)
          controller_enQ(0xFE, 1); // RESEND
          BX_KEY_THIS s.kbd_controller.tim = 1;
        }
        break;

      case 0xf5: // Disable (in stream mode)
        BX_KEY_THIS s.mouse.enable = 0;
        controller_enQ(0xFA, 1); // ACK
        BX_DEBUG(("[mouse] Mouse disabled (stream mode)"));
        break;

      case 0xf6: // Set Defaults
        BX_KEY_THIS s.mouse.sample_rate     = 100; /* reports per second (default) */
        BX_KEY_THIS s.mouse.resolution_cpmm = 4; /* 4 counts per millimeter (default) */
        BX_KEY_THIS s.mouse.scaling         = 1;   /* 1:1 (default) */
        BX_KEY_THIS s.mouse.enable          = 0;
        BX_KEY_THIS s.mouse.mode            = MOUSE_MODE_STREAM;
        controller_enQ(0xFA, 1); // ACK
        BX_DEBUG(("[mouse] Set Defaults"));
        break;

      case 0xff: // Reset
        // is a mouse present?
        if (is_ps2) {
          BX_KEY_THIS s.mouse.sample_rate     = 100; /* reports per second (default) */
          BX_KEY_THIS s.mouse.resolution_cpmm = 4; /* 4 counts per millimeter (default) */
          BX_KEY_THIS s.mouse.scaling         = 1;   /* 1:1 (default) */
          BX_KEY_THIS s.mouse.mode            = MOUSE_MODE_RESET;
          BX_KEY_THIS s.mouse.enable          = 0;
          if (BX_KEY_THIS s.mouse.im_mode)
            BX_INFO(("wheel mouse mode disabled"));
          BX_KEY_THIS s.mouse.im_mode         = 0;
          /* (mch) NT expects an ack here */
          controller_enQ(0xFA, 1); // ACK
          controller_enQ(0xAA, 1); // completion code
          controller_enQ(0x00, 1); // ID code (standard after reset)
          BX_DEBUG(("[mouse] Mouse reset"));
        } else {
          // a mouse isn't present.  We need to return a 0xFE (resend) instead of a 0xFA (ACK)
          controller_enQ(0xFE, 1); // RESEND
          BX_KEY_THIS s.kbd_controller.tim = 1;
        }
        break;

      case 0xe9: // Get mouse information
        // should we ack here? (mch): Yes
        controller_enQ(0xFA, 1); // ACK
        controller_enQ(BX_KEY_THIS s.mouse.get_status_byte(), 1); // status
        controller_enQ(BX_KEY_THIS s.mouse.get_resolution_byte(), 1); // resolution
        controller_enQ(BX_KEY_THIS s.mouse.sample_rate, 1); // sample rate
        BX_DEBUG(("[mouse] Get mouse information"));
        break;

      case 0xeb: // Read Data (send a packet when in Remote Mode)
        controller_enQ(0xFA, 1); // ACK
        // perhaps we should be adding some movement here.
        mouse_enQ_packet( ((BX_KEY_THIS s.mouse.button_status & 0x0f) | 0x08),
          0x00, 0x00, 0x00); // bit3 of first byte always set
        //assumed we really aren't in polling mode, a rather odd assumption.
        BX_ERROR(("[mouse] Warning: Read Data command partially supported."));
        break;

      case 0xbb: // OS/2 Warp 3 uses this command
       BX_ERROR(("[mouse] ignoring 0xbb command"));
       break;

      default:
        // If PS/2 mouse present, send NACK for unknown commands, otherwise ignore
        if (is_ps2) {
          BX_ERROR(("[mouse] kbd_ctrl_to_mouse(): got value of 0x%02x", value));
          kbd_enQ(0xFE); /* send NACK */
        }
    }
  }
}

void
bx_keyb_c::create_mouse_packet(bool force_enq) {
  Bit8u   b1, b2, b3, b4;

  if(BX_KEY_THIS s.mouse_internal_buffer.num_elements && !force_enq)
    return;

  Bit16s delta_x = BX_KEY_THIS s.mouse.delayed_dx;
  Bit16s delta_y = BX_KEY_THIS s.mouse.delayed_dy;
  Bit8u button_state=BX_KEY_THIS s.mouse.button_status | 0x08;

  if(!force_enq && !delta_x && !delta_y) {
    return;
  }

  if(delta_x>254) delta_x=254;
  if(delta_x<-254) delta_x=-254;
  if(delta_y>254) delta_y=254;
  if(delta_y<-254) delta_y=-254;

  b1 = (button_state & 0x0f) | 0x08; // bit3 always set

  if ( (delta_x>=0) && (delta_x<=255) ) {
    b2 = (Bit8u) delta_x;
    BX_KEY_THIS s.mouse.delayed_dx-=delta_x;
    }
  else if ( delta_x > 255 ) {
    b2 = (Bit8u) 0xff;
    BX_KEY_THIS s.mouse.delayed_dx-=255;
    }
  else if ( delta_x >= -256 ) {
    b2 = (Bit8u) delta_x;
    b1 |= 0x10;
    BX_KEY_THIS s.mouse.delayed_dx-=delta_x;
    }
  else {
    b2 = (Bit8u) 0x00;
    b1 |= 0x10;
    BX_KEY_THIS s.mouse.delayed_dx+=256;
    }

  if ( (delta_y>=0) && (delta_y<=255) ) {
    b3 = (Bit8u) delta_y;
    BX_KEY_THIS s.mouse.delayed_dy-=delta_y;
    }
  else if ( delta_y > 255 ) {
    b3 = (Bit8u) 0xff;
    BX_KEY_THIS s.mouse.delayed_dy-=255;
    }
  else if ( delta_y >= -256 ) {
    b3 = (Bit8u) delta_y;
    b1 |= 0x20;
    BX_KEY_THIS s.mouse.delayed_dy-=delta_y;
    }
  else {
    b3 = (Bit8u) 0x00;
    b1 |= 0x20;
    BX_KEY_THIS s.mouse.delayed_dy+=256;
    }

  b4 = (Bit8u) -BX_KEY_THIS s.mouse.delayed_dz;

  mouse_enQ_packet(b1, b2, b3, b4);
}


void
bx_keyb_c::mouse_enabled_changed(bx_bool enabled)
{
#if BX_SUPPORT_PCIUSB
  // if type == usb, connect or disconnect the USB mouse
  if (BX_KEY_THIS s.mouse.type == BX_MOUSE_TYPE_USB) {
    DEV_usb_mouse_enable(enabled);
    return;
  }
#endif

  if (BX_KEY_THIS s.mouse.delayed_dx || BX_KEY_THIS s.mouse.delayed_dy ||
      BX_KEY_THIS s.mouse.delayed_dz) {
    create_mouse_packet(1);
  }
  BX_KEY_THIS s.mouse.delayed_dx=0;
  BX_KEY_THIS s.mouse.delayed_dy=0;
  BX_KEY_THIS s.mouse.delayed_dz=0;
  BX_DEBUG(("PS/2 mouse %s", enabled?"enabled":"disabled"));
}

  void
bx_keyb_c::mouse_motion(int delta_x, int delta_y, int delta_z, unsigned button_state)
{
  bool force_enq=0;

  // If mouse events are disabled on the GUI headerbar, don't
  // generate any mouse data
  if (bx_options.Omouse_enabled->get () == 0)
    return;

  // if type == serial, redirect mouse data to the serial device
  if ((BX_KEY_THIS s.mouse.type == BX_MOUSE_TYPE_SERIAL) ||
      (BX_KEY_THIS s.mouse.type == BX_MOUSE_TYPE_SERIAL_WHEEL)) {
    DEV_serial_mouse_enq(delta_x, delta_y, delta_z, button_state);
    return;
  }

#if BX_SUPPORT_BUSMOUSE
  // if type == bus, redirect mouse data to the bus device
  if (BX_KEY_THIS s.mouse.type == BX_MOUSE_TYPE_BUS) {
    DEV_bus_mouse_enq(delta_x, delta_y, 0, button_state);
    return;
  }
#endif

#if BX_SUPPORT_PCIUSB
  // if an usb mouse is connected redirect mouse data to the usb device
  if (DEV_usb_mouse_connected()) {
    DEV_usb_mouse_enq(delta_x, delta_y, delta_z, button_state);
    return;
  }
#endif

  // don't generate interrupts if we are in remote mode.
  if ( BX_KEY_THIS s.mouse.mode == MOUSE_MODE_REMOTE)
    // is there any point in doing any work if we don't act on the result
    // so go home.
    return;

  // Note: enable only applies in STREAM MODE.
  if ( BX_KEY_THIS s.mouse.enable==0 )
    return;

  // scale down the motion
  if ( (delta_x < -1) || (delta_x > 1) )
    delta_x /= 2;
  if ( (delta_y < -1) || (delta_y > 1) )
    delta_y /= 2;

  if (!BX_KEY_THIS s.mouse.im_mode)
    delta_z = 0;

#ifdef VERBOSE_KBD_DEBUG
  if (delta_x != 0 || delta_y != 0 || delta_z != 0)
    BX_DEBUG(("[mouse] Dx=%d Dy=%d Dz=%d", delta_x, delta_y, delta_z));
#endif  /* ifdef VERBOSE_KBD_DEBUG */

  if( (delta_x==0) && (delta_y==0) && (delta_z==0) && (BX_KEY_THIS s.mouse.button_status == (button_state & 0x7) ) ) {
    BX_DEBUG(("Ignoring useless mouse_motion call:"));
    BX_DEBUG(("This should be fixed in the gui code."));
    return;
  }

  if ((BX_KEY_THIS s.mouse.button_status != (button_state & 0x7)) || delta_z) {
    force_enq=1;
  }

  BX_KEY_THIS s.mouse.button_status = button_state & 0x7;

  if(delta_x>255) delta_x=255;
  if(delta_y>255) delta_y=255;
  if(delta_x<-256) delta_x=-256;
  if(delta_y<-256) delta_y=-256;

  BX_KEY_THIS s.mouse.delayed_dx+=delta_x;
  BX_KEY_THIS s.mouse.delayed_dy+=delta_y;
  BX_KEY_THIS s.mouse.delayed_dz = delta_z;

  if((BX_KEY_THIS s.mouse.delayed_dx>255)||
     (BX_KEY_THIS s.mouse.delayed_dx<-256)||
     (BX_KEY_THIS s.mouse.delayed_dy>255)||
     (BX_KEY_THIS s.mouse.delayed_dy<-256)) {
    force_enq=1;
  }

  create_mouse_packet(force_enq);
}
