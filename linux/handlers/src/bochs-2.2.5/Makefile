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
#
####################################################
# NOTE: To be compatibile with nmake (microsoft vc++) please follow
# the following rules:
#   use $(VAR) not ${VAR}

prefix          = /usr/local
exec_prefix     = ${prefix}
srcdir          = .

bindir          = ${exec_prefix}/bin
libdir          = ${exec_prefix}/lib
plugdir         = ${exec_prefix}/lib/bochs/plugins
mandir          = ${prefix}/man
man1dir         = $(mandir)/man1
man5dir         = $(mandir)/man5
docdir          = $(prefix)/share/doc/bochs
sharedir        = $(prefix)/share/bochs
top_builddir    = .
top_srcdir      = $(srcdir)

DESTDIR =

VERSION=2.2.5
VER_STRING=2.2.5
REL_STRING=Build from CVS snapshot on December 30, 2005
MAN_PAGE_1_LIST=bochs bximage bxcommit bochs-dlx
MAN_PAGE_5_LIST=bochsrc
INSTALL_LIST_SHARE=bios/BIOS-* bios/VGABIOS* 
INSTALL_LIST_DOC=CHANGES COPYING README TODO
INSTALL_LIST_BIN=bochs bximage bxcommit
INSTALL_LIST_BIN_OPTIONAL=bochsdbg
INSTALL_LIST_WIN32=$(INSTALL_LIST_SHARE) $(INSTALL_LIST_DOC) $(INSTALL_LIST_BIN) $(INSTALL_LIST_BIN_OPTIONAL) niclist
INSTALL_LIST_MACOSX=$(INSTALL_LIST_SHARE) $(INSTALL_LIST_DOC) bochs.app bochs.scpt bximage bxcommit
# for win32 and macosx, these files get renamed to *.txt in install process
TEXT_FILE_LIST=README CHANGES COPYING TODO VGABIOS-elpin-LICENSE VGABIOS-lgpl-README
CP=cp
CAT=cat
RM=rm
MV=mv
LN_S=ln -sf
DLXLINUX_TAR=dlxlinux4.tar.gz
DLXLINUX_TAR_URL=http://bochs.sourceforge.net/guestos/$(DLXLINUX_TAR)
DLXLINUX_ROMFILE=BIOS-bochs-latest
GUNZIP=gunzip
WGET=wget
SED=sed
MKDIR=mkdir
RMDIR=rmdir
TAR=tar
CHMOD=chmod
# the GZIP variable is reserved by gzip program
GZIP_BIN=gzip -9
GUNZIP=gunzip
ZIP=zip
UNIX2DOS=unix2dos
LIBTOOL=$(SHELL) $(top_builddir)/libtool
DLLTOOL=dlltool
RC_CMD=

.SUFFIXES: .cc

srcdir = .


SHELL = /bin/sh



CC = gcc
CXX = g++
CFLAGS = -g -O2 -fpermissive -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -pthread  $(X_CFLAGS) `sdl-config --cflags` $(MCH_CFLAGS) $(FLA_FLAGS)  -DBX_SHARE_PATH='"$(sharedir)"'
CXXFLAGS = -g -O2 -fpermissive -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -pthread  $(X_CFLAGS) `sdl-config --cflags` $(MCH_CFLAGS) $(FLA_FLAGS)  -DBX_SHARE_PATH='"$(sharedir)"'

LDFLAGS = 
LIBS =  -lm
# To compile with readline:
#   linux needs just -lreadline
#   solaris needs -lreadline -lcurses
X_LIBS = 
X_PRE_LIBS =  -lSM -lICE
XPM_LIB = -lXpm
GUI_LINK_OPTS_X = $(X_LIBS) $(X_PRE_LIBS) -lX11 $(XPM_LIB)
GUI_LINK_OPTS_SDL = `sdl-config --cflags --libs`
GUI_LINK_OPTS_SVGA =  -lvga -lvgagl
GUI_LINK_OPTS_BEOS = -lbe
GUI_LINK_OPTS_RFB =  
GUI_LINK_OPTS_AMIGAOS = 
GUI_LINK_OPTS_WIN32 = -luser32 -lgdi32 -lcomdlg32 -lcomctl32 -lwsock32
GUI_LINK_OPTS_WIN32_VCPP = user32.lib gdi32.lib winmm.lib \
  comdlg32.lib comctl32.lib wsock32.lib advapi32.lib
GUI_LINK_OPTS_MACOS =
GUI_LINK_OPTS_CARBON = -framework Carbon
GUI_LINK_OPTS_NOGUI =
GUI_LINK_OPTS_TERM = -lncurses
GUI_LINK_OPTS_WX = 
GUI_LINK_OPTS =  $(GUI_LINK_OPTS_X) $(GUI_LINK_OPTS_SDL) $(GUI_LINK_OPTS_RFB) $(GUI_LINK_OPTS_TERM)  
RANLIB = ranlib

CFLAGS_CONSOLE = -g -O2 -fpermissive -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -pthread $(MCH_CFLAGS) $(FLA_FLAGS)
CXXFLAGS_CONSOLE = -g -O2 -fpermissive -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -pthread $(MCH_CFLAGS) $(FLA_FLAGS)
BXIMAGE_LINK_OPTS = 

BX_INCDIRS = -I. -I$(srcdir)/. -Iinstrument/stubs -I$(srcdir)/instrument/stubs

#SUBDIRS = iodev bx_debug

#all install uninstall: config.h#
#        for subdir in $(SUBDIRS); do #
#          echo making $@ in $$subdir; #
#          (cd $$subdir && $(MAKE) $(MDEFINES) $@) || exit 1; #
#        done#



# gnu flags for clean up
#CFLAGS  = -ansi -O -g -Wunused -Wuninitialized


NONINLINE_OBJS = \
	logio.o \
	main.o \
	config.o \
	load32bitOShack.o \
	pc_system.o \
	osdep.o \
	plugin.o \
	

EXTERN_ENVIRONMENT_OBJS = \
	main.o \
	config.o \
	load32bitOShack.o \
	pc_system.o

DEBUGGER_LIB   = bx_debug/libdebug.a
DISASM_LIB     = disasm/libdisasm.a
INSTRUMENT_LIB = instrument/stubs/libinstrument.a
FPU_LIB        = fpu/libfpu.a
READLINE_LIB   = 
EXTRA_LINK_OPTS = 

GDBSTUB_OBJS = gdbstub.o

BX_OBJS = $(NONINLINE_OBJS)

BX_INCLUDES = bochs.h config.h osdep.h


.cc.o:
	$(CXX) -c $(BX_INCDIRS) $(CXXFLAGS) $< -o $@
.c.o:
	$(CC) -c $(BX_INCDIRS) $(CFLAGS) $(FPU_FLAGS) $< -o $@


all: bochs  bximage bxcommit 



bochs: iodev/libiodev.a  \
           cpu/libcpu.a memory/libmemory.a gui/libgui.a \
             $(BX_OBJS) \
           $(SIMX86_OBJS) $(FPU_LIB) $(GDBSTUB_OBJS) 
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) -export-dynamic $(BX_OBJS) $(SIMX86_OBJS) \
		iodev/libiodev.a cpu/libcpu.a memory/libmemory.a gui/libgui.a \
		    \
		$(GDBSTUB_OBJS) $(FPU_LIB) \
		$(GUI_LINK_OPTS) \
		$(MCH_LINK_FLAGS) \
		$(SIMX86_LINK_FLAGS) \
		$(READLINE_LIB) \
		$(EXTRA_LINK_OPTS) \
		$(LIBS)

# Special make target for cygwin/mingw using dlltool instead of
# libtool.  This creates a .DEF file, and exports file, an import library,
# and then links bochs.exe with the exports file.
.win32_dll_plugin_target: iodev/libiodev.a  \
           cpu/libcpu.a memory/libmemory.a gui/libgui.a \
             $(BX_OBJS) \
           $(SIMX86_OBJS) $(FPU_LIB) $(GDBSTUB_OBJS) 
	$(DLLTOOL) --export-all-symbols --output-def bochs.def \
		$(BX_OBJS) $(SIMX86_OBJS) \
		iodev/libiodev.a cpu/libcpu.a memory/libmemory.a gui/libgui.a \
		    \
		$(GDBSTUB_OBJS) $(FPU_LIB)
	$(DLLTOOL) --dllname bochs.exe --def bochs.def --output-lib dllexports.a
	$(DLLTOOL) --dllname bochs.exe --output-exp bochs.exp --def bochs.def
	$(CXX) -o bochs.exe $(CXXFLAGS) $(LDFLAGS) -export-dynamic \
	    $(BX_OBJS) bochs.exp $(SIMX86_OBJS) \
		iodev/libiodev.a cpu/libcpu.a memory/libmemory.a gui/libgui.a \
		    \
		$(GDBSTUB_OBJS) $(FPU_LIB) \
		$(GUI_LINK_OPTS) \
		$(MCH_LINK_FLAGS) \
		$(SIMX86_LINK_FLAGS) \
		$(READLINE_LIB) \
		$(EXTRA_LINK_OPTS) \
		$(LIBS)
	touch .win32_dll_plugin_target

bochs_plugins:
	cd gui && $(MAKE) plugins
	cd iodev && $(MAKE) plugins

bximage: misc/bximage.o
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(CXXFLAGS_CONSOLE) $(LDFLAGS) $(BXIMAGE_LINK_OPTS) misc/bximage.o

bxcommit: misc/bxcommit.o
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(CXXFLAGS_CONSOLE) $(LDFLAGS) misc/bxcommit.o

niclist: misc/niclist.o
	$(LIBTOOL) --mode=link $(CXX) -o $@ $(CXXFLAGS_CONSOLE) $(LDFLAGS) misc/niclist.o

# compile with console CXXFLAGS, not gui CXXFLAGS
misc/bximage.o: $(srcdir)/misc/bximage.c
	$(CC) -c $(BX_INCDIRS) $(CFLAGS_CONSOLE) $(srcdir)/misc/bximage.c -o $@

misc/bxcommit.o: $(srcdir)/misc/bxcommit.c
	$(CC) -c $(BX_INCDIRS) $(CFLAGS_CONSOLE) $(srcdir)/misc/bxcommit.c -o $@

misc/niclist.o: $(srcdir)/misc/niclist.c
	$(CC) -c $(BX_INCDIRS) $(CFLAGS_CONSOLE) $(srcdir)/misc/niclist.c -o $@

$(BX_OBJS): $(BX_INCLUDES)

bxversion.h:
	$(RM) -f bxversion.h
	echo '/////////////////////////////////////////////////////////////////////////' > bxversion.h
	echo '// $$Id: Makefile.in,v 1.175 2005/07/30 15:02:40 vruppert Exp $$' >> bxversion.h
	echo '/////////////////////////////////////////////////////////////////////////' >> bxversion.h
	echo '// This file is generated by "make bxversion.h"' >> bxversion.h
	echo "#define VER_STRING \"$(VER_STRING)\"" >> bxversion.h
	echo "#define REL_STRING \"$(REL_STRING)\"" >> bxversion.h

iodev/libiodev.a::
	cd iodev && \
	$(MAKE) $(MDEFINES) libiodev.a
	echo done

bx_debug/libdebug.a::
	cd bx_debug && \
	$(MAKE) $(MDEFINES) libdebug.a
	echo done

cpu/libcpu.a::
	cd cpu && \
	$(MAKE) $(MDEFINES) libcpu.a
	echo done

memory/libmemory.a::
	cd memory && \
	$(MAKE) $(MDEFINES) libmemory.a
	echo done

gui/libgui.a::
	cd gui && \
	$(MAKE) $(MDEFINES) libgui.a
	echo done

disasm/libdisasm.a::
	cd disasm && \
	$(MAKE) $(MDEFINES) libdisasm.a
	echo done

instrument/stubs/libinstrument.a::
	cd instrument/stubs && \
	$(MAKE) $(MDEFINES) libinstrument.a
	echo done

fpu/libfpu.a::
	cd fpu && \
	$(MAKE) $(MDEFINES) libfpu.a
	echo done

libbochs.a:
	-rm -f libbochs.a
	ar rv libbochs.a $(EXTERN_ENVIRONMENT_OBJS)
	$(RANLIB) libbochs.a

libbochs_cpu.a:  $(BX_OBJS)
	-rm -f libbochs_cpu.a
	ar rv libbochs_cpu.a $(BX_OBJS)
	$(RANLIB) libbochs_cpu.a

# for wxWidgets port, on win32 platform
wxbochs_resources.o: wxbochs.rc
	windres $(srcdir)/wxbochs.rc -o $@ --include-dir=`not_found --prefix`/include

# for win32 gui
win32res.o: win32res.rc
	$(RC_CMD)$@ $(srcdir)/win32res.rc

#####################################################################
# Install target for all platforms.
#####################################################################

install: all install_unix

#####################################################################
# Install target for win32
#
# This is intended to be run in cygwin, since it has better scripting
# tools.
#####################################################################

install_win32: download_dlx 
	-mkdir -p $(prefix)
	cp obj-release/*.exe .
	for i in $(INSTALL_LIST_WIN32); do if test -f $$i; then cp $$i $(prefix); else cp $(srcdir)/$$i $(prefix); fi; done
	cp $(srcdir)/misc/sb16/sb16ctrl.example $(prefix)/sb16ctrl.txt
	cp $(srcdir)/misc/sb16/sb16ctrl.exe $(prefix)
	#cat $(srcdir)/build/win32/DOC-win32.htm | $(SED) -e 's/2.2.5/$(VERSION)/g' > $(prefix)/DOC-win32.htm
	cp $(srcdir)/.bochsrc $(prefix)/bochsrc-sample.txt
	-mkdir $(prefix)/keymaps
	cp $(srcdir)/gui/keymaps/*.map $(prefix)/keymaps
	cat $(DLXLINUX_TAR) | (cd $(prefix) && tar xzvf -)
	echo '..\bochs' > $(prefix)/dlxlinux/start.bat
	dlxrc=$(prefix)/dlxlinux/bochsrc.txt; mv $$dlxrc $$dlxrc.orig && sed < $$dlxrc.orig 's/\/usr\/local\/bochs\/latest/../' > $$dlxrc && rm -f $$dlxrc.orig
	mv $(prefix)/README $(prefix)/README.orig
	cat $(srcdir)/build/win32/README.win32-binary $(prefix)/README.orig > $(prefix)/README
	rm -f $(prefix)/README.orig
	for i in $(TEXT_FILE_LIST); do mv $(prefix)/$$i $(prefix)/$$i.txt; done
	cd $(prefix); $(UNIX2DOS) *.txt */*.txt
	cd $(prefix); NAME=`pwd|$(SED) 's/.*\///'`; (cd ..; $(ZIP) $$NAME.zip -r $$NAME); ls -l ../$$NAME.zip

#####################################################################
# install target for unix
#####################################################################

install_unix: install_bin  install_man install_share install_doc 

install_bin::
	for i in $(DESTDIR)$(bindir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	for i in $(INSTALL_LIST_BIN); do if test -f $$i; then install $$i $(DESTDIR)$(bindir); else install $(srcdir)/$$i $(DESTDIR)$(bindir); fi; done
	-for i in $(INSTALL_LIST_BIN_OPTIONAL); do if test -f $$i; then install $$i $(DESTDIR)$(bindir); else install $(srcdir)/$$i $(DESTDIR)$(bindir); fi; done

install_libtool_plugins::
	for i in $(DESTDIR)$(plugdir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	list=`cd gui && echo *.la`; for i in $$list; do $(LIBTOOL) --mode=install install gui/$$i $(DESTDIR)$(plugdir); done
	list=`cd iodev && echo *.la`; for i in $$list; do $(LIBTOOL) --mode=install install iodev/$$i $(DESTDIR)$(plugdir); done
	$(LIBTOOL) --finish $(DESTDIR)$(plugdir)

install_dll_plugins::
	for i in $(DESTDIR)$(plugdir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	list=`cd gui && echo *.dll`; for i in $$list; do cp gui/$$i $(DESTDIR)$(plugdir); done
	list=`cd iodev && echo *.dll`; for i in $$list; do cp iodev/$$i $(DESTDIR)$(plugdir); done

install_share::
	for i in $(DESTDIR)$(sharedir);	do mkdir -p $$i && test -d $$i && test -w $$i; done
	for i in $(INSTALL_LIST_SHARE); do if test -f $$i; then install -m 644 $$i $(DESTDIR)$(sharedir); else install -m 644 $(srcdir)/$$i $(DESTDIR)$(sharedir); fi; done
	-mkdir $(DESTDIR)$(sharedir)/keymaps
	for i in $(srcdir)/gui/keymaps/*.map; do install -m 644 $$i $(DESTDIR)$(sharedir)/keymaps/; done

install_doc::
	for i in $(DESTDIR)$(docdir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	for i in $(INSTALL_LIST_DOC); do if test -f $$i; then install -m 644 $$i $(DESTDIR)$(docdir); else install -m 644 $(srcdir)/$$i $(DESTDIR)$(docdir); fi; done
	$(RM) -f $(DESTDIR)$(docdir)/README
	$(CAT) $(srcdir)/build/linux/README.linux-binary $(srcdir)/README > $(DESTDIR)$(docdir)/README
	install -m 644 $(srcdir)/.bochsrc $(DESTDIR)$(docdir)/bochsrc-sample.txt


build_docbook::
	cd doc/docbook; make

dl_docbook::
	cd doc/docbook; make dl_docs

install_docbook: build_docbook
	cd doc/docbook; make install

install_man::
	-mkdir -p $(DESTDIR)$(man1dir)
	-mkdir -p $(DESTDIR)$(man5dir)
	for i in $(MAN_PAGE_1_LIST); do cat $(srcdir)/doc/man/$$i.1 | $(SED) 's/@version@/$(VERSION)/g' | $(GZIP_BIN) -c >  $(DESTDIR)$(man1dir)/$$i.1.gz; chmod 644 $(DESTDIR)$(man1dir)/$$i.1.gz; done
	for i in $(MAN_PAGE_5_LIST); do cat $(srcdir)/doc/man/$$i.5 | $(GZIP_BIN) -c >  $(DESTDIR)$(man5dir)/$$i.5.gz; chmod 644 $(DESTDIR)$(man5dir)/$$i.5.gz; done

download_dlx: $(DLXLINUX_TAR)

$(DLXLINUX_TAR):
	$(RM) -f $(DLXLINUX_TAR)
	$(WGET) $(DLXLINUX_TAR_URL)
	test -f $(DLXLINUX_TAR)

unpack_dlx: $(DLXLINUX_TAR)
	rm -rf dlxlinux
	$(GUNZIP) -c $(DLXLINUX_TAR) | $(TAR) -xvf -
	test -d dlxlinux
	(cd dlxlinux; $(MV) bochsrc.txt bochsrc.txt.orig; $(SED) -e "s/1\.1\.2/$(VERSION)/g"  -e 's,/usr/local/bochs/latest,$(prefix)/share/bochs,g' < bochsrc.txt.orig > bochsrc.txt; rm -f bochsrc.txt.orig)

install_dlx:
	$(RM) -rf $(DESTDIR)$(sharedir)/dlxlinux
	cp -r dlxlinux $(DESTDIR)$(sharedir)/dlxlinux
	$(CHMOD) 755 $(DESTDIR)$(sharedir)/dlxlinux
	$(GZIP_BIN) $(DESTDIR)$(sharedir)/dlxlinux/hd10meg.img
	$(CHMOD) 644 $(DESTDIR)$(sharedir)/dlxlinux/*
	for i in bochs-dlx; do cp $(srcdir)/build/linux/$$i $(DESTDIR)$(bindir)/$$i; $(CHMOD) 755 $(DESTDIR)$(bindir)/$$i; done

uninstall::
	$(RM) -rf $(DESTDIR)$(sharedir)
	$(RM) -rf $(DESTDIR)$(docdir)
	$(RM) -rf $(DESTDIR)$(libdir)/bochs
	for i in bochs bximage bxcommit bochs-dlx; do rm -f $(DESTDIR)$(bindir)/$$i; done
	for i in $(MAN_PAGE_1_LIST); do $(RM) -f $(man1dir)/$$i.1.gz; done
	for i in $(MAN_PAGE_5_LIST); do $(RM) -f $(man5dir)/$$i.5.gz; done

V6WORKSPACE_ZIP=build/win32/workspace.zip
V6WORKSPACE_FILES=bochs.dsw bochs.dsp bochs.opt cpu/cpu.dsp \
	memory/memory.dsp iodev/iodev.dsp instrument/stubs/stubs.dsp \
	gui/gui.dsp fpu/fpu.dsp disasm/disasm.dsp bx_debug/bx_debug.dsp \
	misc/niclist.dsp bximage.dsp bxcommit.dsp

v6workspace:
	zip $(V6WORKSPACE_ZIP) $(V6WORKSPACE_FILES)

########
# the win32_snap target is used to create a ZIP of bochs sources configured
# for VC++.  This ZIP is stuck on the website every once in a while to make
# it easier for VC++ users to compile bochs.  First, you should
# run "sh .conf.win32-vcpp" to configure the source code, then do
# "make win32_snap" to unzip the workspace files and create the ZIP.
########
win32_snap:
	unzip $(V6WORKSPACE_ZIP)
	$(SHELL) ./build/win32/cc2cpp
	make zip
	$(SHELL) ./build/win32/cpp2cc

tar:
	NAME=`pwd|$(SED) 's/.*\///'`; (cd ..; $(RM) -f $$NAME.zip; tar cf - $$NAME | $(GZIP_BIN) > $$NAME.tar.gz); ls -l ../$$NAME.tar.gz

zip:
	NAME=`pwd|$(SED) 's/.*\///'`; (cd ..; $(RM) -f $$NAME.zip; $(ZIP) $$NAME.zip -r $$NAME -x \*CVS\* -x \*.cvsignore ); ls -l ../$$NAME.zip

clean:
	rm -f  *.o
	rm -f  *.a
	rm -f  bochs
	rm -f  bochs.exe
	rm -f  bximage
	rm -f  bximage.exe
	rm -f  bxcommit
	rm -f  bxcommit.exe
	rm -f  niclist
	rm -f  niclist.exe
	rm -f  bochs.out
	rm -f  bochsout.txt
	rm -f  bochs.exp
	rm -f  bochs.def
	rm -f  bochs.scpt
	rm -f  -rf bochs.app
	rm -f  -rf .libs
	rm -f  .win32_dll_plugin_target

local-dist-clean: clean
	rm -f  config.h config.status config.log config.cache
	rm -f  .dummy `find . -name '*.dsp' -o -name '*.dsw' -o -name '*.opt' -o -name '.DS_Store'`
	rm -f  bxversion.h build/linux/bochs-dlx _rpm_top *.rpm
	rm -f  build/win32/nsis/Makefile build/win32/nsis/bochs.nsi
	rm -f  build/macosx/Info.plist build/macosx/script_compiled.rsrc
	rm -f  libtool
	rm -f  ltdlconf.h

all-clean: clean
	cd iodev && \
	$(MAKE) clean
	echo done
	cd bx_debug && \
	$(MAKE) clean
	echo done
	cd cpu && \
	$(MAKE) clean
	echo done
	cd memory && \
	$(MAKE) clean
	echo done
	cd gui && \
	$(MAKE) clean
	echo done
	cd disasm && \
	$(MAKE) clean
	echo done
	cd instrument/stubs && \
	$(MAKE) clean
	echo done
	cd misc && \
	$(MAKE) clean
	echo done
	cd fpu && \
	$(MAKE) clean
	echo done
	cd doc/docbook && \
	$(MAKE) clean
	echo done
	cd host/linux/pcidev && \
	$(MAKE) clean
	echo done

dist-clean: local-dist-clean
	cd iodev && \
	$(MAKE) dist-clean
	echo done
	cd bx_debug && \
	$(MAKE) dist-clean
	echo done
	cd bios && \
	$(MAKE) dist-clean
	echo done
	cd cpu && \
	$(MAKE) dist-clean
	echo done
	cd memory && \
	$(MAKE) dist-clean
	echo done
	cd gui && \
	$(MAKE) dist-clean
	echo done
	cd disasm && \
	$(MAKE) dist-clean
	echo done
	cd instrument/stubs && \
	$(MAKE) dist-clean
	echo done
	cd misc && \
	$(MAKE) dist-clean
	echo done
	cd fpu && \
	$(MAKE) dist-clean
	echo done
	cd doc/docbook && \
	$(MAKE) dist-clean
	echo done
	cd host/linux/pcidev && \
	$(MAKE) dist-clean
	echo done
	rm -f  Makefile

###########################################
# Build app on MacOS X
###########################################
MACOSX_STUFF=build/macosx
MACOSX_STUFF_SRCDIR=$(srcdir)/$(MACOSX_STUFF)
APP=bochs.app
APP_PLATFORM=MacOS
SCRIPT_EXEC=bochs.scpt
SCRIPT_DATA=$(MACOSX_STUFF_SRCDIR)/script.data
SCRIPT_R=$(MACOSX_STUFF_SRCDIR)/script.r
SCRIPT_APPLESCRIPT=$(MACOSX_STUFF_SRCDIR)/bochs.applescript
SCRIPT_COMPILED_RSRC=$(MACOSX_STUFF)/script_compiled.rsrc
REZ=/Developer/Tools/Rez
CPMAC=/Developer/Tools/CpMac
RINCLUDES=/System/Library/Frameworks/Carbon.framework/Libraries/RIncludes
REZ_ARGS=-append -i $RINCLUDES -d SystemSevenOrLater=1 -useDF
STANDALONE_LIBDIR=`pwd`/$(APP)/Contents/$(APP_PLATFORM)/lib
OSACOMPILE=/usr/bin/osacompile
SETFILE=/Developer/Tools/Setfile

# On a MacOS X machine, you run rez, osacompile, and setfile to
# produce the script executable, which has both a data fork and a
# resource fork.  Ideally, we would just recompile the whole
# executable at build time, but unfortunately this cannot be done on
# the SF compile farm through an ssh connection because osacompile
# needs to be run locally for some reason.  Solution: If the script
# sources are changed, rebuild the executable on a MacOSX machine, 
# split it into its data and resource forks and check them into CVS 
# as separate files.  Then at release time, all that's left to do is 
# put the data and resource forks back together to make a working script.
# (This can be done through ssh.)
#
# Sources:
# 1. script.r: resources for the script
# 2. script.data: binary data for the script
# 3. bochs.applescript: the source of the script
# 
# NOTE: All of this will fail if you aren't building on an HFS+
# filesystem!  On the SF compile farm building in your user directory
# will fail, while doing the build in /tmp will work ok.

# check if this filesystem supports resource forks at all
test_hfsplus:
	$(RM) -rf test_hfsplus
	echo data > test_hfsplus
	# if you get "Not a directory", then this filesystem doesn't support resources
	echo resource > test_hfsplus/rsrc
	# test succeeded
	$(RM) -rf test_hfsplus

# Step 1 (must be done locally on MacOSX, only when sources change)
# Compile and pull out just the resource fork.  The resource fork is
# checked into CVS as script_compiled.rsrc.  Note that we don't need
# to check in the data fork of tmpscript because it is identical to the
# script.data input file.
$(SCRIPT_COMPILED_RSRC): $(SCRIPT_R) $(SCRIPT_APPLESCRIPT)
	$(RM) -f tmpscript 
	$(CP) -f $(SCRIPT_DATA) tmpscript
	$(REZ) -append $(SCRIPT_R) -o tmpscript
	$(OSACOMPILE) -o tmpscript $(SCRIPT_APPLESCRIPT)
	$(CP) tmpscript/rsrc $(SCRIPT_COMPILED_RSRC)
	$(RM) -f tmpscript

# Step 2 (can be done locally or remotely on MacOSX)
# Combine the data fork and resource fork, and set attributes.
$(SCRIPT_EXEC): $(SCRIPT_DATA) $(SCRIPT_COMPILED_RSRC)
	rm -f $(SCRIPT_EXEC)
	$(CP) $(SCRIPT_DATA) $(SCRIPT_EXEC)
	if test ! -f $(SCRIPT_COMPILED_RSRC); then $(CP) $(srcdir)/$(SCRIPT_COMPILED_RSRC) $(SCRIPT_COMPILED_RSRC); fi
	$(CP) $(SCRIPT_COMPILED_RSRC) $(SCRIPT_EXEC)/rsrc
	$(SETFILE) -t "APPL" -c "aplt" $(SCRIPT_EXEC)

$(APP)/.build: bochs test_hfsplus $(SCRIPT_EXEC)
	rm -f $(APP)/.build
	$(MKDIR) -p $(APP)
	$(MKDIR) -p $(APP)/Contents
	$(CP) -f $(MACOSX_STUFF)/Info.plist $(APP)/Contents
	$(CP) -f $(MACOSX_STUFF_SRCDIR)/pbdevelopment.plist $(APP)/Contents
	echo -n "APPL????"  > $(APP)/Contents/PkgInfo
	$(MKDIR) -p $(APP)/Contents/$(APP_PLATFORM)
	$(CP) bochs $(APP)/Contents/$(APP_PLATFORM)
	$(MKDIR) -p $(APP)/Contents/Resources
	$(REZ) $(REZ_ARGS) $(MACOSX_STUFF_SRCDIR)/bochs.r -o $(APP)/Contents/Resources/bochs.rsrc
	$(CP) -f $(MACOSX_STUFF_SRCDIR)/bochs-icn.icns $(APP)/Contents/Resources
	ls -ld $(APP) $(SCRIPT_EXEC) $(SCRIPT_EXEC)/rsrc
	touch $(APP)/.build

$(APP)/.build_plugins: $(APP)/.build bochs_plugins
	rm -f $(APP)/.build_plugins
	$(MKDIR) -p $(STANDALONE_LIBDIR);
	list=`cd gui && echo *.la`; for i in $$list; do $(LIBTOOL) cp gui/$$i $(STANDALONE_LIBDIR); done;
	list=`cd iodev && echo *.la`; for i in $$list; do $(LIBTOOL) cp iodev/$$i $(STANDALONE_LIBDIR); done;
	$(LIBTOOL) --finish $(STANDALONE_LIBDIR);
	touch $(APP)/.build_plugins

install_macosx: all download_dlx 
	-mkdir -p $(prefix)
	for i in $(INSTALL_LIST_MACOSX); do if test -e $$i; then $(CPMAC) -r $$i $(prefix); else $(CPMAC) -r $(srcdir)/$$i $(prefix); fi; done
	$(CPMAC) $(srcdir)/.bochsrc $(prefix)/bochsrc-sample.txt
	-mkdir $(prefix)/keymaps
	$(CPMAC) $(srcdir)/gui/keymaps/*.map $(prefix)/keymaps
	cat $(DLXLINUX_TAR) | (cd $(prefix) && tar xzvf -)
	dlxrc=$(prefix)/dlxlinux/bochsrc.txt; mv "$$dlxrc" "$$dlxrc.orig" && sed < "$$dlxrc.orig" 's/\/usr\/local\/bochs\/latest/../' > "$$dlxrc" && rm -f "$$dlxrc.orig"
	mv $(prefix)/README $(prefix)/README.orig
	cat $(srcdir)/build/macosx/README.macosx-binary $(prefix)/README.orig > $(prefix)/README
	rm -f $(prefix)/README.orig
	$(CPMAC) $(SCRIPT_EXEC) $(prefix)/dlxlinux
	for i in $(TEXT_FILE_LIST); do mv $(prefix)/$$i $(prefix)/$$i.txt; done

###########################################
# BeOS make target.
# Build the binary normally, then copy the resource attributes.
###########################################
.bochs_beos_target: bochs
	unzip $(srcdir)/build/beos/resource.zip
	copyattr -t ICON BeBochs.rsrc bochs 
	copyattr -t MICN BeBochs.rsrc bochs 

###########################################
# dependencies generated by
#  gcc -MM -I. -Iinstrument/stubs *.cc | sed -e 's/\.cc/.cc/g' -e 's,cpu/,cpu/,g'
###########################################
config.o: config.cc bochs.h config.h osdep.h bx_debug/debug.h bxversion.h \
  gui/siminterface.h cpu/cpu.h cpu/lazy_flags.h cpu/hostasm.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h gui/textconfig.h gui/keymap.h \
  instrument/stubs/instrument.h iodev/iodev.h iodev/pci.h iodev/pci2isa.h \
  iodev/pci_ide.h iodev/pcivga.h iodev/vga.h iodev/biosdev.h iodev/cmos.h \
  iodev/dma.h iodev/floppy.h iodev/harddrv.h iodev/keyboard.h \
  iodev/parallel.h iodev/pic.h iodev/pit.h iodev/pit_wrap.h \
  iodev/pit82c54.h iodev/virt_timer.h iodev/serial.h iodev/sb16.h \
  iodev/unmapped.h iodev/ne2k.h iodev/guest2host.h iodev/slowdown_timer.h \
  iodev/extfpuirq.h iodev/gameport.h
gdbstub.o: gdbstub.cc bochs.h config.h osdep.h bx_debug/debug.h \
  bxversion.h gui/siminterface.h cpu/cpu.h cpu/lazy_flags.h cpu/hostasm.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h gui/textconfig.h gui/keymap.h \
  instrument/stubs/instrument.h iodev/iodev.h iodev/pci.h iodev/pci2isa.h \
  iodev/pci_ide.h iodev/pcivga.h iodev/vga.h iodev/biosdev.h iodev/cmos.h \
  iodev/dma.h iodev/floppy.h iodev/harddrv.h iodev/keyboard.h \
  iodev/parallel.h iodev/pic.h iodev/pit.h iodev/pit_wrap.h \
  iodev/pit82c54.h iodev/virt_timer.h iodev/serial.h iodev/sb16.h \
  iodev/unmapped.h iodev/ne2k.h iodev/guest2host.h iodev/slowdown_timer.h \
  iodev/extfpuirq.h iodev/gameport.h
load32bitOShack.o: load32bitOShack.cc bochs.h config.h osdep.h \
  bx_debug/debug.h bxversion.h gui/siminterface.h cpu/cpu.h \
  cpu/lazy_flags.h cpu/hostasm.h cpu/icache.h cpu/apic.h cpu/i387.h \
  fpu/softfloat.h fpu/tag_w.h fpu/status_w.h fpu/control_w.h cpu/xmm.h \
  memory/memory.h pc_system.h plugin.h extplugin.h ltdl.h gui/gui.h \
  gui/textconfig.h gui/keymap.h instrument/stubs/instrument.h \
  iodev/iodev.h iodev/pci.h iodev/pci2isa.h iodev/pci_ide.h \
  iodev/pcivga.h iodev/vga.h iodev/biosdev.h iodev/cmos.h iodev/dma.h \
  iodev/floppy.h iodev/harddrv.h iodev/keyboard.h iodev/parallel.h \
  iodev/pic.h iodev/pit.h iodev/pit_wrap.h iodev/pit82c54.h \
  iodev/virt_timer.h iodev/serial.h iodev/sb16.h iodev/unmapped.h \
  iodev/ne2k.h iodev/guest2host.h iodev/slowdown_timer.h \
  iodev/extfpuirq.h iodev/gameport.h
logio.o: logio.cc iodev/iodev.h bochs.h config.h osdep.h bx_debug/debug.h \
  bxversion.h gui/siminterface.h cpu/cpu.h cpu/lazy_flags.h cpu/hostasm.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h gui/textconfig.h gui/keymap.h \
  instrument/stubs/instrument.h iodev/pci.h iodev/pci2isa.h \
  iodev/pci_ide.h iodev/pcivga.h iodev/vga.h iodev/biosdev.h iodev/cmos.h \
  iodev/dma.h iodev/floppy.h iodev/harddrv.h iodev/keyboard.h \
  iodev/parallel.h iodev/pic.h iodev/pit.h iodev/pit_wrap.h \
  iodev/pit82c54.h iodev/virt_timer.h iodev/serial.h iodev/sb16.h \
  iodev/unmapped.h iodev/ne2k.h iodev/guest2host.h iodev/slowdown_timer.h \
  iodev/extfpuirq.h iodev/gameport.h
main.o: main.cc bochs.h config.h osdep.h bx_debug/debug.h bxversion.h \
  gui/siminterface.h cpu/cpu.h cpu/lazy_flags.h cpu/hostasm.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h gui/textconfig.h gui/keymap.h \
  instrument/stubs/instrument.h iodev/iodev.h iodev/pci.h iodev/pci2isa.h \
  iodev/pci_ide.h iodev/pcivga.h iodev/vga.h iodev/biosdev.h iodev/cmos.h \
  iodev/dma.h iodev/floppy.h iodev/harddrv.h iodev/keyboard.h \
  iodev/parallel.h iodev/pic.h iodev/pit.h iodev/pit_wrap.h \
  iodev/pit82c54.h iodev/virt_timer.h iodev/serial.h iodev/sb16.h \
  iodev/unmapped.h iodev/ne2k.h iodev/guest2host.h iodev/slowdown_timer.h \
  iodev/extfpuirq.h iodev/gameport.h
osdep.o: osdep.cc bochs.h config.h osdep.h bx_debug/debug.h bxversion.h \
  gui/siminterface.h cpu/cpu.h cpu/lazy_flags.h cpu/hostasm.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h gui/textconfig.h gui/keymap.h \
  instrument/stubs/instrument.h
pc_system.o: pc_system.cc bochs.h config.h osdep.h bx_debug/debug.h \
  bxversion.h gui/siminterface.h cpu/cpu.h cpu/lazy_flags.h cpu/hostasm.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h gui/textconfig.h gui/keymap.h \
  instrument/stubs/instrument.h iodev/iodev.h iodev/pci.h iodev/pci2isa.h \
  iodev/pci_ide.h iodev/pcivga.h iodev/vga.h iodev/biosdev.h iodev/cmos.h \
  iodev/dma.h iodev/floppy.h iodev/harddrv.h iodev/keyboard.h \
  iodev/parallel.h iodev/pic.h iodev/pit.h iodev/pit_wrap.h \
  iodev/pit82c54.h iodev/virt_timer.h iodev/serial.h iodev/sb16.h \
  iodev/unmapped.h iodev/ne2k.h iodev/guest2host.h iodev/slowdown_timer.h \
  iodev/extfpuirq.h iodev/gameport.h
plex86-interface.o: plex86-interface.cc bochs.h config.h osdep.h \
  bx_debug/debug.h bxversion.h gui/siminterface.h cpu/cpu.h \
  cpu/lazy_flags.h cpu/hostasm.h cpu/icache.h cpu/apic.h cpu/i387.h \
  fpu/softfloat.h fpu/tag_w.h fpu/status_w.h fpu/control_w.h cpu/xmm.h \
  memory/memory.h pc_system.h plugin.h extplugin.h ltdl.h gui/gui.h \
  gui/textconfig.h gui/keymap.h instrument/stubs/instrument.h \
  plex86-interface.h plex86/plex86.h plex86/descriptor.h
plugin.o: plugin.cc bochs.h config.h osdep.h bx_debug/debug.h bxversion.h \
  gui/siminterface.h cpu/cpu.h cpu/lazy_flags.h cpu/hostasm.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h gui/textconfig.h gui/keymap.h \
  instrument/stubs/instrument.h iodev/iodev.h iodev/pci.h iodev/pci2isa.h \
  iodev/pci_ide.h iodev/pcivga.h iodev/vga.h iodev/biosdev.h iodev/cmos.h \
  iodev/dma.h iodev/floppy.h iodev/harddrv.h iodev/keyboard.h \
  iodev/parallel.h iodev/pic.h iodev/pit.h iodev/pit_wrap.h \
  iodev/pit82c54.h iodev/virt_timer.h iodev/serial.h iodev/sb16.h \
  iodev/unmapped.h iodev/ne2k.h iodev/guest2host.h iodev/slowdown_timer.h \
  iodev/extfpuirq.h iodev/gameport.h
