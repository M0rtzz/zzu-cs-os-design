# To build an NSIS installer, get NSIS version 2.0a7 from
# http://sourceforge.net/projects/nsis
# Fix the MAKENSIS variable so that you have the correct path.
# Unzip the windows binary release into a subdirectory of this 
# directory, for example "2.0.pre2".  Make sure the VERSION
# variable has the same name as the directory, and in bochs.nsi
# the VER_MAJOR, VER_MINOR, and VER_REV values should also match.
#
# Type make, and it should build an installer called Bochs-${VERSION}.exe

MAKENSIS='c:/Program Files/NSIS/makensis'

VERSION=2.2.5
TARGET=Bochs-${VERSION}.exe
DLXDIR=bochs-${VERSION}/dlxlinux

all: ${TARGET}

fixups::
	if test -f ${DLXDIR}/bochsrc.txt; then mv ${DLXDIR}/bochsrc.txt ${DLXDIR}/bochsrc.bxrc; fi
	rm -f ${DLXDIR}/*.bat

${TARGET}: fixups bochs.nsi
	rm -rf ${TARGET}
	${MAKENSIS} bochs.nsi
	test -f ${TARGET}
	ls -l ${TARGET}

clean::
	rm -rf ${TARGET}
