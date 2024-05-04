# Copyright (C) 2002  MandrakeSoft S.A.
#
#   MandrakeSoft S.A.
#   43, rue d'Aboukir
#   75002 Paris - France
#   http://www.linux-mandrake.com/
#   http://www.mandrakesoft.com/
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

# Makefile for the gui component of bochs


.SUFFIXES: .cc

prefix          = /usr/local
exec_prefix     = ${prefix}
srcdir          = .

bindir          = ${exec_prefix}/bin
libdir          = ${exec_prefix}/lib
mandir          = ${prefix}/man
man1dir         = $(mandir)/man1
man5dir         = $(mandir)/man5
docdir          = $(prefix)/share/doc/bochs
sharedir        = $(prefix)/share/bochs
top_builddir    = ..
top_srcdir      = ..

SHELL = /bin/sh



CXX = g++
CXXFLAGS = $(BX_INCDIRS) -g -O2 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -pthread   $(X_CFLAGS) `sdl-config --cflags`
LOCAL_CXXFLAGS =
LDFLAGS = 
LIBS =  -lm
X_CFLAGS = 
RANLIB = ranlib
PLUGIN_PATH=${exec_prefix}/lib
top_builddir    = ..
LIBTOOL=$(SHELL) $(top_builddir)/libtool
WIN32_DLL_IMPORT_LIBRARY=../dllexports.a
BX_INCDIRS = -I.. -I$(srcdir)/.. -I../iodev -I$(srcdir)/../iodev -I../instrument/stubs -I$(srcdir)/../instrument/stubs

GUI_OBJS_X11  = x.o
GUI_OBJS_SDL = sdl.o
GUI_OBJS_SVGA = svga.o
GUI_OBJS_BEOS = beos.o
GUI_OBJS_WIN32 = win32.o
GUI_OBJS_MACOS = macintosh.o
GUI_OBJS_CARBON = carbon.o
GUI_OBJS_NOGUI = nogui.o
GUI_OBJS_TERM  = term.o
GUI_OBJS_RFB = rfb.o
GUI_OBJS_AMIGAOS = amigaos.o
GUI_OBJS_WX = wx.o
GUI_OBJS_WX_SUPPORT = wxmain.o wxdialog.o
OBJS_THAT_CANNOT_BE_PLUGINS = keymap.o gui.o siminterface.o textconfig.o 
OBJS_THAT_CAN_BE_PLUGINS = 

X_LIBS = 
X_PRE_LIBS =  -lSM -lICE
GUI_LINK_OPTS_X = $(X_LIBS) $(X_PRE_LIBS) -lX11 -lXpm
GUI_LINK_OPTS_SDL = `sdl-config --cflags --libs`
GUI_LINK_OPTS_SVGA =  -lvga -lvgagl
GUI_LINK_OPTS_BEOS = -lbe
GUI_LINK_OPTS_RFB =  
GUI_LINK_OPTS_AMIGAOS = 
GUI_LINK_OPTS_WIN32 = -luser32 -lgdi32 -lcomdlg32 -lcomctl32
GUI_LINK_OPTS_WIN32_VCPP = user32.lib gdi32.lib winmm.lib \
  comdlg32.lib comctl32.lib wsock32.lib
GUI_LINK_OPTS_MACOS =
GUI_LINK_OPTS_CARBON = -framework Carbon
GUI_LINK_OPTS_NOGUI =
GUI_LINK_OPTS_TERM = -lncurses
GUI_LINK_OPTS_WX = 
GUI_LINK_OPTS =  $(GUI_LINK_OPTS_X) $(GUI_LINK_OPTS_SDL) $(GUI_LINK_OPTS_RFB) $(GUI_LINK_OPTS_TERM)  

NONPLUGIN_OBJS = $(OBJS_THAT_CANNOT_BE_PLUGINS) $(OBJS_THAT_CAN_BE_PLUGINS) $(OBJS_THAT_SUPPORT_OTHER_PLUGINS)  $(GUI_OBJS_X11) $(GUI_OBJS_SDL) $(GUI_OBJS_RFB) $(GUI_OBJS_TERM) $(GUI_OBJS_NOGUI) 
PLUGIN_OBJS = 

#
# -------- end configurable options --------------------------
#

all: libgui.a 

plugins: $(PLUGIN_OBJS:%.o=libbx_%.la)

libgui.a: $(NONPLUGIN_OBJS)
	rm -f  libgui.a
	ar rv $@ $(NONPLUGIN_OBJS)
	ranlib libgui.a

# standard compile rule for C++ files
.cc.o:
	$(CXX) -c  $(CXXFLAGS) $(LOCAL_CXXFLAGS) $< -o $@

##### building plugins with libtool
%.lo: %.cc
	$(LIBTOOL) --mode=compile $(CXX) -c $(CXXFLAGS) $(LOCAL_CXXFLAGS) $< -o $@

libbx_%.la: %.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH)

libbx_x.la: x.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_X)

libbx_sdl.la: sdl.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_SDL)

libbx_svga.la: svga.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_SVGA)

libbx_beos.la: beos.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_BEOS)

libbx_rfb.la: rfb.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_RFB)

libbx_amigaos.la: amigaos.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_AMIGAOS)

libbx_win32.la: win32.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_WIN32)

libbx_macos.la: macos.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_MACOS)

libbx_carbon.la: carbon.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_CARBON)

libbx_nogui.la: nogui.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_NOGUI)

libbx_term.la: term.lo
	$(LIBTOOL) --mode=link $(CXX) -module $< -o $@ -rpath $(PLUGIN_PATH) $(GUI_LINK_OPTS_TERM)

# special link rules for plugins that require more than one object file
libbx_wx.la: $(GUI_OBJS_WX:.o=.lo) $(GUI_OBJS_WX_SUPPORT:.o=.lo)
	$(LIBTOOL) --mode=link $(CXX) -module $(GUI_OBJS_WX:.o=.lo) $(GUI_OBJS_WX_SUPPORT:.o=.lo) -o libbx_wx.la -rpath $(PLUGIN_PATH)  $(GUI_LINK_OPTS_WX)

#### building DLLs for win32  (tested on cygwin only)
bx_%.dll: %.o
	$(CXX) $(CXXFLAGS) -shared -o $@ $< $(WIN32_DLL_IMPORT_LIBRARY)  $(GUI_LINK_OPTS_WIN32)

bx_wx.dll: $(GUI_OBJS_WX) $(GUI_OBJS_WX_SUPPORT)
	$(CXX) $(CXXFLAGS) -shared -o bx_wx.dll $(GUI_OBJS_WX) $(GUI_OBJS_WX_SUPPORT) $(WIN32_DLL_IMPORT_LIBRARY) `wx-config --libs` -luser32 -lgdi32 -lcomdlg32 -lcomctl32

bx_sdl.dll: $(GUI_OBJS_SDL) 
	$(CXX) $(CXXFLAGS) -shared -o bx_sdl.dll $(GUI_OBJS_SDL) $(WIN32_DLL_IMPORT_LIBRARY) $(GUI_LINK_OPTS_SDL)

bx_rfb.dll: $(GUI_OBJS_RFB) 
	$(CXX) $(CXXFLAGS) -shared -o bx_rfb.dll $(GUI_OBJS_RFB) $(WIN32_DLL_IMPORT_LIBRARY) $(GUI_LINK_OPTS_RFB)

# no need to build DLLs for beos.o
# no need to build DLLs for x.o

##### end DLL section

clean:
	rm -f  -rf .libs *.la *.a *.lo *.o *.dll

dist-clean: clean
	rm -f  Makefile

###########################################
# all other dependencies generated by
#  gcc -MM -I.. -I../iodev -I../instrument/stubs `wx-config --cxxflags` *.cc | \
#     sed -e 's/\.cc/.cc/g'
#  gcc -MM -I.. -I../iodev -I../instrument/stubs `wx-config --cxxflags` *.cc | \
#     sed -e 's/\.cc/.cc/g' -e 's/\.o:/.lo:/g'
#
# This means that every source file is listed twice, once with a .o rule
# and then again with an identical .lo rule.  The .lo rules are used when
# building plugins.
###########################################
amigaos.o: amigaos.cc ../bochs.h ../config.h ../osdep.h \
  ../bx_debug/debug.h ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h \
  ../cpu/lazy_flags.h ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h \
  ../cpu/i387.h ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h \
  ../fpu/control_w.h ../cpu/xmm.h ../memory/memory.h ../pc_system.h \
  ../plugin.h ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
beos.o: beos.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
carbon.o: carbon.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
gui.o: gui.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h ../gui/bitmaps/floppya.h ../gui/bitmaps/floppyb.h \
  ../gui/bitmaps/mouse.h ../gui/bitmaps/reset.h ../gui/bitmaps/power.h \
  ../gui/bitmaps/snapshot.h ../gui/bitmaps/copy.h ../gui/bitmaps/paste.h \
  ../gui/bitmaps/configbutton.h ../gui/bitmaps/cdromd.h \
  ../gui/bitmaps/userbutton.h
keymap.o: keymap.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h
macintosh.o: macintosh.cc ../bochs.h ../config.h ../osdep.h \
  ../bx_debug/debug.h ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h \
  ../cpu/lazy_flags.h ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h \
  ../cpu/i387.h ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h \
  ../fpu/control_w.h ../cpu/xmm.h ../memory/memory.h ../pc_system.h \
  ../plugin.h ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
nogui.o: nogui.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h icon_bochs.h
rfb.o: rfb.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.h ../font/vga.bitmap.h rfb.h
sdl.o: sdl.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.h sdl.h sdlkeys.h
siminterface.o: siminterface.cc ../bochs.h ../config.h ../osdep.h \
  ../bx_debug/debug.h ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h \
  ../cpu/lazy_flags.h ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h \
  ../cpu/i387.h ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h \
  ../fpu/control_w.h ../cpu/xmm.h ../memory/memory.h ../pc_system.h \
  ../plugin.h ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
svga.o: svga.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
term.o: term.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.h
textconfig.o: textconfig.cc ../config.h ../osdep.h textconfig.h \
  siminterface.h ../extplugin.h ../ltdl.h
win32.o: win32.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
win32dialog.o: win32dialog.cc ../config.h
wx.o: wx.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h ../font/vga.bitmap.h wxmain.h
wxdialog.o: wxdialog.cc ../config.h ../osdep.h ../gui/siminterface.h \
  ../bxversion.h wxdialog.h wxmain.h
wxmain.o: wxmain.cc ../config.h ../osdep.h ../gui/siminterface.h \
  ../bxversion.h wxdialog.h wxmain.h ../extplugin.h ../ltdl.h \
  bitmaps/cdromd.xpm bitmaps/copy.xpm bitmaps/floppya.xpm \
  bitmaps/floppyb.xpm bitmaps/paste.xpm bitmaps/power.xpm \
  bitmaps/reset.xpm bitmaps/snapshot.xpm bitmaps/mouse.xpm \
  bitmaps/userbutton.xpm icon_bochs.xpm
x.o: x.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.xpm ../font/vga.bitmap.h
amigaos.lo: amigaos.cc ../bochs.h ../config.h ../osdep.h \
  ../bx_debug/debug.h ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h \
  ../cpu/lazy_flags.h ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h \
  ../cpu/i387.h ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h \
  ../fpu/control_w.h ../cpu/xmm.h ../memory/memory.h ../pc_system.h \
  ../plugin.h ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
beos.lo: beos.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
carbon.lo: carbon.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
gui.lo: gui.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h ../gui/bitmaps/floppya.h ../gui/bitmaps/floppyb.h \
  ../gui/bitmaps/mouse.h ../gui/bitmaps/reset.h ../gui/bitmaps/power.h \
  ../gui/bitmaps/snapshot.h ../gui/bitmaps/copy.h ../gui/bitmaps/paste.h \
  ../gui/bitmaps/configbutton.h ../gui/bitmaps/cdromd.h \
  ../gui/bitmaps/userbutton.h
keymap.lo: keymap.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h
macintosh.lo: macintosh.cc ../bochs.h ../config.h ../osdep.h \
  ../bx_debug/debug.h ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h \
  ../cpu/lazy_flags.h ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h \
  ../cpu/i387.h ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h \
  ../fpu/control_w.h ../cpu/xmm.h ../memory/memory.h ../pc_system.h \
  ../plugin.h ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
nogui.lo: nogui.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h icon_bochs.h
rfb.lo: rfb.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.h ../font/vga.bitmap.h rfb.h
sdl.lo: sdl.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.h sdl.h sdlkeys.h
siminterface.lo: siminterface.cc ../bochs.h ../config.h ../osdep.h \
  ../bx_debug/debug.h ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h \
  ../cpu/lazy_flags.h ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h \
  ../cpu/i387.h ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h \
  ../fpu/control_w.h ../cpu/xmm.h ../memory/memory.h ../pc_system.h \
  ../plugin.h ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
svga.lo: svga.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
term.lo: term.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.h
textconfig.lo: textconfig.cc ../config.h ../osdep.h textconfig.h \
  siminterface.h ../extplugin.h ../ltdl.h
win32.lo: win32.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h
win32dialog.lo: win32dialog.cc ../config.h
wx.lo: wx.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h ../font/vga.bitmap.h wxmain.h
wxdialog.lo: wxdialog.cc ../config.h ../osdep.h ../gui/siminterface.h \
  ../bxversion.h wxdialog.h wxmain.h
wxmain.lo: wxmain.cc ../config.h ../osdep.h ../gui/siminterface.h \
  ../bxversion.h wxdialog.h wxmain.h ../extplugin.h ../ltdl.h \
  bitmaps/cdromd.xpm bitmaps/copy.xpm bitmaps/floppya.xpm \
  bitmaps/floppyb.xpm bitmaps/paste.xpm bitmaps/power.xpm \
  bitmaps/reset.xpm bitmaps/snapshot.xpm bitmaps/mouse.xpm \
  bitmaps/userbutton.xpm icon_bochs.xpm
x.lo: x.cc ../bochs.h ../config.h ../osdep.h ../bx_debug/debug.h \
  ../bxversion.h ../gui/siminterface.h ../cpu/cpu.h ../cpu/lazy_flags.h \
  ../cpu/hostasm.h ../cpu/icache.h ../cpu/apic.h ../cpu/i387.h \
  ../fpu/softfloat.h ../fpu/tag_w.h ../fpu/status_w.h ../fpu/control_w.h \
  ../cpu/xmm.h ../memory/memory.h ../pc_system.h ../plugin.h \
  ../extplugin.h ../ltdl.h ../gui/gui.h ../gui/textconfig.h \
  ../gui/keymap.h ../instrument/stubs/instrument.h ../iodev/iodev.h \
  ../iodev/pci.h ../iodev/pci2isa.h ../iodev/pci_ide.h ../iodev/pcivga.h \
  ../iodev/vga.h ../iodev/biosdev.h ../iodev/cmos.h ../iodev/dma.h \
  ../iodev/floppy.h ../iodev/harddrv.h ../iodev/keyboard.h \
  ../iodev/parallel.h ../iodev/pic.h ../iodev/pit.h ../iodev/pit_wrap.h \
  ../iodev/pit82c54.h ../iodev/virt_timer.h ../iodev/serial.h \
  ../iodev/sb16.h ../iodev/unmapped.h ../iodev/ne2k.h \
  ../iodev/guest2host.h ../iodev/slowdown_timer.h ../iodev/extfpuirq.h \
  ../iodev/gameport.h icon_bochs.xpm ../font/vga.bitmap.h
