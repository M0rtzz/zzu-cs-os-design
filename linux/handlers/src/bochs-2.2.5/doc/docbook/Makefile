###################################################################
# doc/docbook/Makefile
# $Id: Makefile.in,v 1.20 2005/12/06 22:15:50 vruppert Exp $
#
# Builds documentation in various formats from SGML source, and
# allows easy update to the Bochs web site.
#
###################################################################

prefix          = /usr/local
srcdir          = .
docdir          = $(prefix)/share/doc/bochs
DOCBOOK2HTML = not_found
WGET = wget
TARGZ = bochsdoc.tar.gz
RENDERED_DOC_URL = http://bochs.sf.net/doc/docbook/$(TARGZ)

DESTDIR=

# name of the major documentation sections
SECTIONS=user documentation development
SECTION_HTML=$(SECTIONS:%=%/index.html)

# these files get installed in addition to the sections
EXTRAS=index.html images

# complete list of what to install
INSTALL_LIST=$(SECTIONS) $(EXTRAS)

# ssh to this server to install the docs
REMOTE_HOST=shell.sf.net

# path of preexisting install on the remote server.  Each section
# will go into a subdirectory of $REMOTE_PATH, as in
# $REMOTE_PATH/user.
REMOTE_PATH=/home/groups/b/bo/bochs/htdocs/doc/docbook

# -x means don't try to forward X authorization, it won't work for SF
SSH=ssh -x

all: $(SECTION_HTML)

user/index.html: $(srcdir)/user/user.dbk
	-mkdir -p user
	$(DOCBOOK2HTML) -o user $(srcdir)/user/user.dbk

documentation/index.html: $(srcdir)/documentation/documentation.dbk
	-mkdir -p documentation
	$(DOCBOOK2HTML) -o documentation $(srcdir)/documentation/documentation.dbk 

development/index.html: $(srcdir)/development/development.dbk
	-mkdir -p development
	$(DOCBOOK2HTML) -o development $(srcdir)/development/development.dbk

fixperm::
	# fix permissions locally so that tar will install things right
	chmod 664 `find $(INSTALL_LIST) -type f -print`
	chmod 775 `find $(INSTALL_LIST) -type d -print`

copy_from_srcdir::
	for i in $(EXTRAS); do if test -e $(srcdir)/$$i -a ! -e $$i; then cp -r $(srcdir)/$$i $$i; fi; done

$(TARGZ): all copy_from_srcdir fixperm
	rm -f $(TARGZ)
	tar cf - $(INSTALL_LIST) --exclude=CVS --exclude=.cvsignore --exclude=*.dbk | gzip > $(TARGZ)

install: all copy_from_srcdir
	for i in $(DESTDIR)$(docdir); do if test ! -d $$i; then mkdir -p $$i; fi; done
	#for i in $(INSTALL_LIST); do cp -r $$i $(DESTDIR)$(docdir); done
	for i in $(INSTALL_LIST); do tar cf - --exclude=CVS --exclude=.cvsignore --exclude=*.dbk $$i | ( cd $(DESTDIR)$(docdir); tar xf - ); done

test_sfuser:
	@if test "$$SFUSER" = ""; then SFUSER=`whoami`; export SFUSER; fi; \
	  echo Your Source Forge username is $${SFUSER}.
	@echo 'If this is not correct, set the environment variable $$SFUSER.'

# Install the stuff on the remote server using ssh.  It will assume that your
# local username is the same as your Source Forge username, unless you define
# an environment variable SFUSER.
webinst: $(TARGZ)
	# copy to remote
	@echo Installing documentation on $(REMOTE_PATH)
	if test "$$SFUSER" = ""; then SFUSER=`whoami`; export SFUSER; fi; \
	  scp $(TARGZ) $${SFUSER}@$(REMOTE_HOST):$(REMOTE_PATH); \
	  $(SSH) $${SFUSER}@$(REMOTE_HOST) "cd $(REMOTE_PATH) && umask 002 && gunzip -c $(TARGZ) | tar xvf -"

# Download rendered docs in a TAR file from the Bochs web site using wget.
# This is useful for getting documentation onto platforms that don't have
# docbook tools.
dl_docs::
	rm -f $(TARGZ)
	$(WGET) $(RENDERED_DOC_URL)
	gunzip -c $(TARGZ) | tar xvf -
	touch */*.html

clean:
	# remove generated files
	for S in $(SECTIONS); do \
	  rm -f $$S/*.html $$S/*.htm $$S/*.ps $$S/*.pdf $$S/*.out; \
	done
	rm -f  $(TARGZ)

dist-clean: clean
	rm -f  Makefile
