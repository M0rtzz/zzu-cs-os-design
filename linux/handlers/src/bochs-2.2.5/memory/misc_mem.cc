/////////////////////////////////////////////////////////////////////////
// $Id: misc_mem.cc,v 1.70 2005/12/27 16:59:27 vruppert Exp $
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
//  I/O memory handlers API Copyright (C) 2003 by Frank Cornelis
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


#include "iodev/iodev.h"
#define LOG_THIS BX_MEM(0)->

#if BX_PROVIDE_CPU_MEMORY

Bit32u BX_MEM_C::get_memory_in_k(void)
{
  return(BX_MEM_THIS megabytes * 1024);
}

  // BX_MEM_C constructor
BX_MEM_C::BX_MEM_C(void)
{
  char mem[6];
  snprintf(mem, 6, "MEM%d", BX_SIM_ID);
  put(mem);
  settype(MEMLOG);

  vector = NULL;
  actual_vector = NULL;
  len    = 0;
  megabytes = 0;

  memory_handlers = NULL;
}

void BX_CPP_AttrRegparmN(2)
BX_MEM_C::alloc_vector_aligned (size_t bytes, size_t alignment)
{
  if (actual_vector != NULL) {
    BX_INFO (("freeing existing memory vector"));
    delete [] actual_vector;
    actual_vector = NULL;
    vector = NULL;
  }
  Bit64u test_mask = alignment - 1;
  actual_vector = new Bit8u [(unsigned int)(bytes+test_mask)];
  // round address forward to nearest multiple of alignment.  Alignment 
  // MUST BE a power of two for this to work.
  Bit64u masked = ((Bit64u)(actual_vector + test_mask)) & ~test_mask;
  vector = (Bit8u *)masked;
  // sanity check: no lost bits during pointer conversion
  BX_ASSERT (sizeof(masked) >= sizeof(vector));
  // sanity check: after realignment, everything fits in allocated space
  BX_ASSERT (vector+bytes <= actual_vector+bytes+test_mask);
  BX_INFO (("allocated memory at %p. after alignment, vector=%p", 
	actual_vector, vector));
}

// BX_MEM_C destructor
BX_MEM_C::~BX_MEM_C(void)
{
  if (this-> vector != NULL) {
    delete [] actual_vector;
    actual_vector = NULL;
    vector = NULL;
    delete [] memory_handlers;
    memory_handlers = NULL;
  }
  else {
    BX_DEBUG(("(%u)   memory not freed as it wasn't allocated!", BX_SIM_ID));
  }
}

void BX_MEM_C::init_memory(int memsize)
{
  int idx;

  BX_DEBUG(("Init $Id: misc_mem.cc,v 1.70 2005/12/27 16:59:27 vruppert Exp $"));
  // you can pass 0 if memory has been allocated already through
  // the constructor, or the desired size of memory if it hasn't
  // BX_INFO(("%.2fMB", (float)(BX_MEM_THIS megabytes) ));

  if (BX_MEM_THIS vector == NULL) {
    // memory not already allocated, do now...
    alloc_vector_aligned (memsize+ BIOSROMSZ + EXROMSIZE  + 4096, BX_MEM_VECTOR_ALIGN);
    BX_MEM_THIS len    = memsize;
    BX_MEM_THIS megabytes = memsize / (1024*1024);
    BX_MEM_THIS memory_handlers = new struct memory_handler_struct *[1024 * 1024];
    BX_MEM_THIS rom = &BX_MEM_THIS vector[memsize];
    BX_MEM_THIS bogus = &BX_MEM_THIS vector[memsize + BIOSROMSZ + EXROMSIZE];
    memset(BX_MEM_THIS rom, 0xff, BIOSROMSZ + EXROMSIZE);
    memset(BX_MEM_THIS bogus, 0xff, 4096);
    for (idx = 0; idx < 1024 * 1024; idx++)
	    BX_MEM_THIS memory_handlers[idx] = NULL;
    for (idx = 0; idx < 65; idx++)
      BX_MEM_THIS rom_present[idx] = 0;
    BX_INFO(("%.2fMB", (float)(BX_MEM_THIS megabytes) ));
  }

#if BX_DEBUGGER
  if (megabytes > BX_MAX_DIRTY_PAGE_TABLE_MEGS) {
    BX_INFO(("Error: memory larger than dirty page table can handle"));
    BX_PANIC(("Error: increase BX_MAX_DIRTY_PAGE_TABLE_MEGS"));
  }
#endif

}

#if BX_SMP_PROCESSORS > 1
void put_8bit(Bit8u **pp, Bit8u value)
{
  Bit8u *p = *pp;
  *p++ = value;
  *pp = p;
}

void put_16bit(Bit8u **pp, Bit16u value)
{
  Bit8u *p = *pp;
  *p++ = value & 0xff;
  *p++ = (value >> 8) & 0xff;
  *pp = p;
}

void put_32bit(Bit8u **pp, Bit32u value)
{
  Bit8u *p = *pp;
  *p++ = value & 0xff;
  *p++ = (value >> 8) & 0xff;
  *p++ = (value >> 16) & 0xff;
  *p++ = (value >> 24) & 0xff;
  *pp = p;
}

void put_string(Bit8u **pp, const char *str)
{
  Bit8u *p = *pp;
  while (*str)
    *p++ = *str++;
  *pp = p;
}

Bit8u mp_checksum(const Bit8u *p, int len)
{
  Bit8u sum = 0;
  for (int i = 0; i < len; i++)
    sum += p[i];
  return (Bit8u)(-sum);
}
#endif

//
// Values for type:
//   0 : System Bios
//   1 : VGA Bios
//   2 : Optional ROM Bios
//

void BX_MEM_C::load_ROM(const char *path, Bit32u romaddress, Bit8u type)
{
  struct stat stat_buf;
  int fd, ret, i, start_idx, end_idx;
  unsigned long size, max_size, offset;
  bx_bool is_bochs_bios = 0;

  if (*path == '\0') {
    if (type == 2) {
      BX_PANIC(( "ROM: Optional ROM image undefined"));
    }
    else if (type == 1) {
      BX_PANIC(( "ROM: VGA BIOS image undefined"));
    }
    else {
      BX_PANIC(( "ROM: System BIOS image undefined"));
    }
    return;
  }
  // read in ROM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    if (type < 2) {
      BX_PANIC(( "ROM: couldn't open ROM image file '%s'.", path));
    }
    else {
      BX_ERROR(( "ROM: couldn't open ROM image file '%s'.", path));
    }
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    if (type < 2) {
      BX_PANIC(( "ROM: couldn't stat ROM image file '%s'.", path));
    }
    else {
      BX_ERROR(( "ROM: couldn't stat ROM image file '%s'.", path));
    }
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  if (type > 0) {
    max_size = 0x10000;
  } else {
    max_size = BIOSROMSZ;
  }
  if (size > max_size) {
    close(fd);
    BX_PANIC(("ROM: ROM image too large"));
    return;
  }
  if (type == 0) {
    if (romaddress > 0) {
      if ((romaddress + size) != 0x100000 && (romaddress + size)) {
        close(fd);
        BX_PANIC(("ROM: System BIOS must end at 0xfffff"));
        return;
      }
    } else {
      romaddress = (Bit32u)-size;
    }
    offset = romaddress & BIOS_MASK;
    if ((romaddress & 0xf0000) < 0xf0000) {
      BX_MEM_THIS rom_present[64] = 1;
    }
    is_bochs_bios = (strstr(path, "BIOS-bochs-latest") != NULL);
  } else {
    if ((size % 512) != 0) {
      close(fd);
      BX_PANIC(("ROM: ROM image size must be multiple of 512 (size = %ld)", size));
      return;
    }
    if ((romaddress % 2048) != 0) {
      close(fd);
      BX_PANIC(("ROM: ROM image must start at a 2k boundary"));
      return;
    }
    if ((romaddress < 0xc0000) ||
        (((romaddress + size) > 0xdffff) && (romaddress != 0xe0000))) {
      close(fd);
      BX_PANIC(("ROM: ROM address space out of range"));
      return;
    }
    start_idx = ((romaddress - 0xc0000) >> 11);
    end_idx = start_idx + (size >> 11) + (((size % 2048) > 0) ? 1 : 0);
    if (romaddress < 0xe0000) {
      offset = (romaddress & EXROM_MASK) + BIOSROMSZ;
    } else {
      offset = romaddress & BIOS_MASK;
    }
    for (i = start_idx; i < end_idx; i++) {
      if (BX_MEM_THIS rom_present[i]) {
        close(fd);
        BX_PANIC(("ROM: address space 0x%x already in use", (i * 2048) + 0xc0000));
        return;
      } else {
        BX_MEM_THIS rom_present[i] = 1;
      }
    }
  }
  while (size > 0) {
    ret = read(fd, (bx_ptr_t) &BX_MEM_THIS rom[offset], size);
    if (ret <= 0) {
      BX_PANIC(( "ROM: read failed on BIOS image: '%s'",path));
    }
    size -= ret;
    offset += ret;
  }
  close(fd);
  offset -= (unsigned long)stat_buf.st_size;
  if ((romaddress != 0xe0000) || ((rom[offset] == 0x55) && (rom[offset] == 0xaa))) {
    Bit8u checksum = 0;
    for (i = 0; i < stat_buf.st_size; i++) {
      checksum += BX_MEM_THIS rom[offset + i];
    }
    if (checksum != 0) {
      if (type == 1) {
        BX_PANIC(( "ROM: checksum error in VGABIOS image: '%s'", path));
      } else if (is_bochs_bios) {
        BX_ERROR(( "ROM: checksum error in BIOS image: '%s'", path));
      }
    }
  }
#if BX_SMP_PROCESSORS > 1
  if (is_bochs_bios) {
    Bit8u* pcmp_ptr = &BX_MEM_THIS rom[0xFB000 & BIOS_MASK];
    Bit8u* p = pcmp_ptr;
    put_string(&p, "PCMP"); // signature
    put_16bit(&p, 0); // table length
    put_8bit(&p, 4); // version
    put_8bit(&p, 0); // checksum
    put_string(&p, "BOCHSCPU"); // OEM ID
    put_string(&p, "0.1         "); // vendor ID
    put_32bit(&p, 0); // OEM table pointer
    put_16bit(&p, 0); // OEM table size
    put_16bit(&p, 20); // entry count
    put_32bit(&p, 0xfee00000); // local APIC addr
    put_16bit(&p, 0); // ext table length
    put_8bit(&p, 0); // ext table checksum
    put_8bit(&p, 0); // reserved
    for (i = 0; i < BX_SMP_PROCESSORS; i++) {
      put_8bit(&p, 0); // entry type = processor
      put_8bit(&p, (Bit8u)i); // APIC id
      put_8bit(&p, 0x11); // local APIC version number
      put_8bit(&p, (i==0)?3:1); // cpu flags: enabled, cpu0 = bootstrap cpu
      put_8bit(&p, 0); // cpu signature
      put_8bit(&p, 0);
      put_8bit(&p, 0);
      put_8bit(&p, 0);
      put_16bit(&p, 0x201); // feature flags
      put_16bit(&p, 0);
      put_16bit(&p, 0); // reserved
      put_16bit(&p, 0);
      put_16bit(&p, 0);
      put_16bit(&p, 0);
    }
    put_8bit(&p, 1); // entry type = bus
    put_8bit(&p, 0); // bus ID
    put_string(&p, "ISA   ");
    Bit8u ioapic_id = BX_SMP_PROCESSORS;
    put_8bit(&p, 2); // entry type = I/O APIC
    put_8bit(&p, ioapic_id); // apic id
    put_8bit(&p, 0x11); // I/O APIC version number
    put_8bit(&p, 1); // enabled
    put_32bit(&p, 0xfec00000); // I/O APIC addr
    for (i = 0; i < 16; i++) {
      put_8bit(&p, 3); // entry type = I/O interrupt
      put_8bit(&p, 0); // interrupt type = vectored interrupt
      put_8bit(&p, 0); // flags: po=0, el=0
      put_8bit(&p, 0);
      put_8bit(&p, 0); // source bus ID = ISA
      put_8bit(&p, i); // source bus IRQ
      put_8bit(&p, ioapic_id); // dest I/O APIC ID
      put_8bit(&p, i); // dest I/O APIC interrupt in
    }
    Bit16u len = (Bit16u)(p - pcmp_ptr);
    pcmp_ptr[4] = (Bit8u)len;
    pcmp_ptr[5] = (Bit8u)(len >> 8);
    pcmp_ptr[7] = mp_checksum(pcmp_ptr, len);
    Bit8u *fl_mp_ptr = &BX_MEM_THIS rom[0xFB000 & BIOS_MASK] + ((len + 15) & ~15);
    p = fl_mp_ptr;
    put_string(&p, "_MP_");
    put_32bit(&p, 0xFB000); // pointer to MP config table
    put_8bit(&p, 1); // length in 16 byte units
    put_8bit(&p, 4); // MP spec revision
    put_8bit(&p, 0); // checksum
    put_8bit(&p, 0); // MP feature bytes 1-5
    put_8bit(&p, 0);
    put_8bit(&p, 0);
    put_8bit(&p, 0);
    put_8bit(&p, 0);
    fl_mp_ptr[10] = mp_checksum(fl_mp_ptr, (int)(p - fl_mp_ptr));
  }
#endif
  BX_INFO(("rom at 0x%05x/%u ('%s')",
			(unsigned) romaddress,
			(unsigned) stat_buf.st_size,
 			path));
}

void BX_MEM_C::load_RAM(const char *path, Bit32u ramaddress, Bit8u type)
{
  struct stat stat_buf;
  int fd, ret;
  unsigned long size, offset;

  if (*path == '\0') {
    BX_PANIC(( "RAM: Optional RAM image undefined"));
    return;
    }
  // read in RAM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    BX_PANIC(( "RAM: couldn't open RAM image file '%s'.", path));
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    BX_PANIC(( "RAM: couldn't stat RAM image file '%s'.", path));
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  offset = ramaddress;
  while (size > 0) {
    ret = read(fd, (bx_ptr_t) &BX_MEM_THIS vector[offset], size);
    if (ret <= 0) {
      BX_PANIC(( "RAM: read failed on RAM image: '%s'",path));
    }
    size -= ret;
    offset += ret;
  }
  close(fd);
  BX_INFO(("ram at 0x%05x/%u ('%s')",
			(unsigned) ramaddress,
			(unsigned) stat_buf.st_size,
 			path));
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if ( BX_DEBUGGER || BX_DISASM || BX_GDBSTUB)
  bx_bool
BX_MEM_C::dbg_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf)
{
  bx_bool ret = 1;

  for (; len>0; len--) {
    if ( (addr & 0xfffe0000) == 0x000a0000 )
      *buf = DEV_vga_mem_read(addr);
#if BX_SUPPORT_PCI
    else if ( bx_options.Oi440FXSupport->get () &&
          ((addr & 0xfffc0000) == 0x000c0000) )
    {
      switch (DEV_pci_rd_memtype (addr)) {
        case 0x0:  // Read from ROM
          if ( (addr & 0xfffe0000) == 0x000e0000 )
          {
            *buf = rom[addr & BIOS_MASK];
          }
          else
          {
            *buf = rom[(addr & EXROM_MASK) + BIOSROMSZ];
          }
          break;
        case 0x1:  // Read from ShadowRAM
          *buf = vector[addr];
          break;
        default:
          BX_PANIC(("dbg_fetch_mem: default case"));
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if (addr < BX_MEM_THIS len)
    {
      if ( (addr & 0xfffc0000) != 0x000c0000 ) {
        *buf = vector[addr];
      }
      else if ( (addr & 0xfffe0000) == 0x000e0000 )
      {
        *buf = rom[addr & BIOS_MASK];
      }
      else
      {
        *buf = rom[(addr & EXROM_MASK) + BIOSROMSZ];
      }
    }
    else if (addr >= ~BIOS_MASK)
    {
      *buf = rom[addr & BIOS_MASK];
    }
    else
    {
      *buf = 0xff;
      ret = 0; // error, beyond limits of memory
    }
    buf++;
    addr++;
  }
  return ret;
}
#endif

#if BX_DEBUGGER || BX_GDBSTUB
  bx_bool
BX_MEM_C::dbg_set_mem(Bit32u addr, unsigned len, Bit8u *buf)
{
  if ( (addr + len) > this->len ) {
    return(0); // error, beyond limits of memory
  }
  for (; len>0; len--) {
    if ( (addr & 0xfffe0000) == 0x000a0000 )
      DEV_vga_mem_write(addr, *buf);
#if BX_SUPPORT_PCI
    else if ( bx_options.Oi440FXSupport->get () &&
          ((addr & 0xfffc0000) == 0x000c0000) )
    {
      switch (DEV_pci_wr_memtype (addr)) {
        case 0x0:  // Ignore write to ROM
          break;
        case 0x1:  // Write to ShadowRAM
          vector[addr] = *buf;
          break;
        default:
          BX_PANIC(("dbg_fetch_mem: default case"));
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if ( (addr & 0xfffc0000) != 0x000c0000 && (addr < ~BIOS_MASK) )
    {
      vector[addr] = *buf;
    }
    buf++;
    addr++;
  }
  return(1);
}
#endif

  bx_bool
BX_MEM_C::dbg_crc32(unsigned long (*f)(unsigned char *buf, int len),
    Bit32u addr1, Bit32u addr2, Bit32u *crc)
{
  *crc = 0;
  if (addr1 > addr2)
    return(0);

  if (addr2 >= this->len) {
    return(0); // error, specified address past last phy mem addr
  }
  
  unsigned len = 1 + addr2 - addr1;
  *crc = f(vector + addr1, len);

  return(1);
}

//
// Return a host address corresponding to the guest physical memory
// address (with A20 already applied), given that the calling
// code will perform an 'op' operation.  This address will be
// used for direct access to guest memory as an acceleration by
// a few instructions, like REP {MOV, INS, OUTS, etc}.
// Values of 'op' are { BX_READ, BX_WRITE, BX_RW }.
//
// The other assumption is that the calling code _only_ accesses memory
// directly within the page that encompasses the address requested.
//

  Bit8u * BX_CPP_AttrRegparmN(3)
BX_MEM_C::getHostMemAddr(BX_CPU_C *cpu, Bit32u a20Addr, unsigned op)
{

#if BX_SUPPORT_APIC
    bx_generic_apic_c *local_apic = &cpu->local_apic;
    if (local_apic->get_base () == (a20Addr & ~0xfff))
      return(NULL); // Vetoed!  APIC address space
    bx_generic_apic_c *ioapic = bx_devices.ioapic;
    if (ioapic->get_base () == (a20Addr & ~0xfff))
      return(NULL); // Vetoed!  IOAPIC address space
#endif

  struct memory_handler_struct *memory_handler = memory_handlers[a20Addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20Addr &&
        memory_handler->end >= a20Addr) {
      return(NULL); // Vetoed!  memory handler for vram, mmio and PCI PnP
    }
    memory_handler = memory_handler->next;
  }

  if (op == BX_READ) {
    if ( (a20Addr & 0xfffe0000) == 0x000a0000 )
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
#if BX_SUPPORT_PCI
    else if ( bx_options.Oi440FXSupport->get () &&
             ((a20Addr & 0xfffc0000) == 0x000c0000) )
    {
      switch (DEV_pci_rd_memtype (a20Addr)) {
        case 0x0:   // Read from ROM
          if ( (a20Addr & 0xfffe0000) == 0x000e0000 )
          {
            return( (Bit8u *) & rom[a20Addr & BIOS_MASK]);
          }
          else
          {
            return( (Bit8u *) & rom[(a20Addr & EXROM_MASK) + BIOSROMSZ]);
          }
          break;
        case 0x1:   // Read from ShadowRAM
          return( (Bit8u *) & vector[a20Addr]);
        default:
          BX_PANIC(("getHostMemAddr(): default case"));
          return(0);
      }
    }
#endif
    else if (a20Addr < BX_MEM_THIS len)
    {
      if ( (a20Addr & 0xfffc0000) != 0x000c0000 ) {
        return( (Bit8u *) & vector[a20Addr]);
      }
      else if ( (a20Addr & 0xfffe0000) == 0x000e0000 )
      {
        return( (Bit8u *) & rom[a20Addr & BIOS_MASK]);
      }
      else
      {
        return( (Bit8u *) & rom[(a20Addr & EXROM_MASK) + BIOSROMSZ]);
      }
    }
    else if (a20Addr >= (Bit32u)~BIOS_MASK)
    {
      return( (Bit8u *) & rom[a20Addr & BIOS_MASK]);
    }
    else
    {
      // Error, requested addr is out of bounds.
      return( (Bit8u *) & bogus[a20Addr & 0x0fff]);
    }
  }
  else
  { // op == {BX_WRITE, BX_RW}
    Bit8u *retAddr;
    if ( a20Addr >= BX_MEM_THIS len )
      return(NULL); // Error, requested addr is out of bounds.
    else if ( (a20Addr & 0xfffe0000) == 0x000a0000 )
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
    else if (a20Addr >= (Bit32u)~BIOS_MASK)
      return(NULL); // Vetoed!  ROMs
#if BX_SUPPORT_PCI
    else if ( bx_options.Oi440FXSupport->get () &&
             ((a20Addr & 0xfffc0000) == 0x000c0000) )
    {
      switch (DEV_pci_wr_memtype (a20Addr)) {
        case 0x0:   // Vetoed!  ROMs
          return(NULL);
        case 0x1:   // Write to ShadowRAM
          retAddr = (Bit8u *) & vector[a20Addr];
          break;
        default:
          BX_PANIC(("getHostMemAddr(): default case"));
          return(0);
      }
    }
#endif
    else
    {
      if ( (a20Addr & 0xfffc0000) != 0x000c0000 ) {
        retAddr = (Bit8u *) & vector[a20Addr];
      }
      else
      {
        return(NULL);  // Vetoed!  ROMs
      }
    }

#if BX_SUPPORT_ICACHE
    pageWriteStampTable.decWriteStamp(a20Addr);
#endif

    return(retAddr);
  }
}

/*
 * One needs to provide both a read_handler and a write_handler.
 * XXX: maybe we should check for overlapping memory handlers
 */
  bx_bool 
BX_MEM_C::registerMemoryHandlers(memory_handler_t read_handler, void *read_param,
		memory_handler_t write_handler, void *write_param,
		unsigned long begin_addr, unsigned long end_addr)
{
  if (end_addr < begin_addr)
    return false;
  if (!read_handler)
    return false;
  if (!write_handler)
    return false;
  for (unsigned page_idx = begin_addr >> 20; page_idx <= end_addr >> 20; page_idx++) {
    struct memory_handler_struct *memory_handler = new struct memory_handler_struct;
    memory_handler->next = memory_handlers[page_idx];
    memory_handlers[page_idx] = memory_handler;
    memory_handler->read_handler = read_handler;
    memory_handler->write_handler = write_handler;
    memory_handler->read_param = read_param;
    memory_handler->write_param = write_param;
    memory_handler->begin = begin_addr;
    memory_handler->end = end_addr;
  }
  return true;
}

  bx_bool 
BX_MEM_C::unregisterMemoryHandlers(memory_handler_t read_handler, memory_handler_t write_handler,
		unsigned long begin_addr, unsigned long end_addr)
{
   bx_bool ret = true;
   for (unsigned page_idx = begin_addr >> 20; page_idx <= end_addr >> 20; page_idx++) {
     struct memory_handler_struct *memory_handler = memory_handlers[page_idx];
     struct memory_handler_struct *prev = NULL;
     while (memory_handler && 
         memory_handler->read_handler != read_handler &&
         memory_handler->write_handler != write_handler && 
         memory_handler->begin != begin_addr && 
         memory_handler->end != end_addr)
     {
       prev = memory_handler;
       memory_handler = memory_handler->next;
     }
     if (!memory_handler) {
       ret = false; // we should have found it
       continue; // anyway, try the other pages
     }
     if (prev)
       prev->next = memory_handler->next;
     else
       memory_handlers[page_idx] = memory_handler->next;
     delete memory_handler;
   }  
   return ret;
}
