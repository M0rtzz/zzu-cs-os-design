#!/bin/sh

lock_file="./hdc.img.lock"

if [ -f "${lock_file}" ]; then
    rm "${lock_file}"
fi

export TOOLS_PATH=$(dirname `which $0`)

if [ ! -e "hdc.img" ]; then
tar -xvJf hdc.tar.xz
fi

if [ "$1" ] && [ "$1" = "-m" ]
then
(cd ../linux-0.12; make clean; make; cp Image ../tools/Image)
elif [ "$1" ] && [ "$1" = "-g" ]
then
${TOOLS_PATH}/bochs/bochs-gdb -q -f ${TOOLS_PATH}/bochs/bochsrc-gdb.bxrc & \
gdb -x ${TOOLS_PATH}/bochs/.gdbrc ../linux-0.12/tools/system
else
bochs -q -f ${TOOLS_PATH}/bochs/bochsrc.bxrc
fi