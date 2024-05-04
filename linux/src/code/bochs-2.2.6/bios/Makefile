# Copyright (C) 2001  MandrakeSoft S.A.
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

# Makefile for the BIOS component of bochs


.SUFFIXES: .cc

srcdir = .


SHELL = /bin/sh



CXX = g++
CXXFLAGS = -g -O2 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -pthread

LDFLAGS = 
LIBS =  -lm
RANLIB = ranlib

BCC = bcc
GCC = gcc
AS86 = as86

BX_INCDIRS = -I.. -I$(srcdir)/.. -I../iodev -I$(srcdir)/../iodev
LOCAL_CXXFLAGS =

BUILDDATE = `date '+%m/%d/%y'`
BIOS_BUILD_DATE = "-DBIOS_BUILD_DATE=\"$(BUILDDATE)\""
#
# -------- end configurable options --------------------------
#


.cc.o:
	$(CXX) -c $(BX_INCDIRS) $(CXXFLAGS) $(LOCAL_CXXFLAGS) $< -o $@


bios: biossums BIOS-bochs-latest 

clean:
	rm -f  *.o *.a *.s _rombios*_.c rombios*.txt rombios*.sym
	rm -f   usage biossums

dist-clean: clean
	rm -f  Makefile

bios-clean:
	rm -f  BIOS-bochs-*

BIOS-bochs-latest: rombios.c apmbios.S biossums
	$(GCC) $(BIOS_BUILD_DATE) -E -P $< > _rombios_.c
	$(BCC) -o rombios.s -C-c -D__i86__ -0 -S _rombios_.c
	sed -e 's/^\.text//' -e 's/^\.data//' rombios.s > _rombios_.s
	$(AS86) _rombios_.s -b tmp.bin -u- -w- -g -0 -j -O -l rombios.txt
	-perl ${srcdir}/makesym.perl < rombios.txt > rombios.sym
	mv tmp.bin BIOS-bochs-latest
	./biossums BIOS-bochs-latest
	rm -f _rombios_.s

biossums: biossums.c
	$(GCC) -o biossums biossums.c
