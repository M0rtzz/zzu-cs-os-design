#!/usr/bin/sudo /bin/zsh

# ------------------------------------------------------------------
# @file: mount.sh
# @brief: 挂载文件系统，此文件系统是linux-0.11的文件系统映像，但不影响使用，在此不过多赘述
# @author: M0rtzz
# @date: 2024-05-01
# ------------------------------------------------------------------

mount_folder="hdc"

if [ ! -d "${mount_folder}" ]; then
    mkdir "${mount_folder}"
fi

export OSLAB_PATH=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)
mount -t minix -o loop,offset=1024 ${OSLAB_PATH}/hdc.img ${OSLAB_PATH}/hdc
