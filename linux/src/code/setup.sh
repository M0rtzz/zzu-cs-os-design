#!/usr/bin/sudo /bin/bash

# 如果在编译bochs时遇到libgtk2.0-dev 依赖问题，请手动执行
#   sudo apt-get install aptitude && sudo aptitude install libgtk2.0-dev
# 若old-releases.ubuntu.com源下载过慢，可以使用国内源http://mirrors.ustc.edu.cn/ubuntu-old-releases/

_echo_info(){
    echo -e "[Info]: ${1}"
}

_echo_err(){
    echo -e "\033[31m[Error]: ${1} \033[0m"
}

_echo_succ(){
    echo -e "\033[32m[Succ]: ${1} \033[0m"
}

env_install(){

    sudo apt-get install -y make bin86 gcc-multilib &> /dev/null \
    && _echo_succ "bin86 is installed." || _echo_err "bin86 is not installed!!!"

    if [ ! -z `which gcc-3.4` ];then
        _echo_succ "Gcc-3.4 is installed."
        return
    fi

    GCC_DIR="gcc-3.4"

    DOWNLOAD_LIST=(
        "gcc-3.4-base_3.4.6-6ubuntu3_amd64.deb"
        "gcc-3.4_3.4.6-6ubuntu3_amd64.deb"
        "cpp-3.4_3.4.6-6ubuntu3_amd64.deb"
        "g++-3.4_3.4.6-6ubuntu3_amd64.deb"
        "libstdc++6-dev_3.4.6-6ubuntu3_amd64.deb"
    )

    if [ -z `which gcc-3.4` ]; then
        _echo_info "Start installing gcc-3.4..."
        for deb in ${DOWNLOAD_LIST[*]}; do
            if [ ! -e ${GCC_DIR}/${deb} ]; then
                # wget http://old-releases.ubuntu.com/ubuntu/pool/universe/g/gcc-3.4/${deb} -P ${GCC_DIR} -q --show-progress && \
                wget http://mirrors.ustc.edu.cn/ubuntu-old-releases/ubuntu/pool/universe/g/gcc-3.4/${deb} -P ${GCC_DIR} -q --show-progress && \
                _echo_info "Download ${deb} Sucessfully." || ( rm ${deb} & _echo_err "Download ${deb} unsuccessfully!!!" )
            fi
        done
        sudo dpkg -i ${GCC_DIR}/*.deb &> /dev/null
        sudo apt-get install -y -f &> /dev/null
        if [ ! -z `which gcc-3.4` ];then
            _echo_succ "gcc-3.4 is installed."
        fi
        rm -rf ${GCC_DIR}
    fi
}

bochs_install(){

    sudo apt-get install -y build-essential libgtk2.0-dev \
        libx11-dev xserver-xorg-dev xorg-dev g++ \
        pkg-config libxcursor-dev libxrandr-dev \
        libxinerama-dev libxi-dev &> /dev/null

    sudo apt-get install -y build-essential &> /dev/null
    sudo apt-get install -y bochs bochs-x bochs-sdl &> /dev/null
    
    # INFO:https://github.com/oldlinux-web/oldlinux-files/blob/master/bochs/README_FIRST
    # NOTE: M0rtzz have resolved the bug in the source code
    # if [ ! -e "bochs-2.2.6.tar.gz" ]; then
    #     # wget https://downloads.sourceforge.net/project/bochs/bochs/2.6.9/bochs-2.2.6.tar.gz -q --show-progress  && \
    #     wget https://sourceforge.net/projects/bochs/files/bochs/2.6.9/bochs-2.2.6.tar.gz -q --show-progress  && \
    #     _echo_succ "Download bochs-2.2.6.tar.gz Sucessfully." || (rm bochs-2.2.6.tar.gz & _echo_err "Download bochs-2.2.6.tar.gz unsuccessfully!!!" )
    # fi

    # if [ ! -d "bochs-2.2.6" ];then
    #     tar zxvf bochs-2.2.6.tar.gz &> /dev/null && \
    #     _echo_info "tar bochs-2.2.6.tar.gz Sucessfullyhttp://mirrors.ustc.edu.cn/ubuntu-old-releases/." || \
    #     (rm -rf ../bochs-2.2.6 & _echo_err "tar bochs-2.2.6.tar.gz unsuccessfully!!!" )
    # fi

    if [ -d "bochs-2.2.6" ];then
        cd bochs-2.2.6
        make clean
        if [ "$1" ] && [ "$1" = "-d" ];then
        # sudo apt-get install aptitude && sudo apt install libgtk2.0-dev
        sudo apt update && sudo apt install libgtk2.0-dev
        # ./configure --enable-gdb-stub --enable-disasm 
        ./configure --enable-gdb-stub --enable-new-pit --enable-all-optimizations --enable-4meg-pages --enable-global-pages --enable-pae --enable-sep --enable-cpu-level=6 --enable-sse=2 --disable-reset-on-triple-fault --with-all-libs
        # ./configure --enable-debugger --enable-disasm
        # ./configure --enable-disasm --enable-debugger --enable-new-pit --enable-all-optimizations --enable-4meg-pages --enable-global-pages --enable-pae --enable-sep --enable-cpu-level=6 --enable-sse=2 --disable-reset-on-triple-fault --with-all-libs
        make -j$(nproc) && (cp bochs ./bochsdbg & cp bochs ../bochs-gdb & cp bochs ../../../oslab/bochs/bochs-gdb & _echo_succ "make bochs sucessfully.") && sudo make install || _echo_err "make bochs unsucessfully.!!!"
        else
        ./configure --enable-gdb-stub --enable-new-pit --enable-all-optimizations --enable-4meg-pages --enable-global-pages --enable-pae --enable-sep --enable-cpu-level=6 --enable-sse=2 --disable-reset-on-triple-fault --with-all-libs
        make -j$(nproc) && (cp bochs ./bochsdbg & cp bochs ../bochs-gdb & cp bochs ../../../oslab/bochs/bochs-gdb & _echo_succ "make bochs sucessfully.") && sudo make install || _echo_err "make bochs unsucessfully.!!!"
        fi
    fi

}

trap 'onCtrlC' INT
function onCtrlC () {
    _echo_err "[Warning]: stopped by user."
    rm -rf ${GCC_DIR}
    exit
}
echo " 须知"
# echo "     请根据oslab/README.txt中的第2点，修改下载的bochs-2.2.6源码，并重新执行该脚本"
echo "脚本将完成以下两件事："
echo "     1. 为系统安装相应的编译环境（make，bin86，gcc-3.4，gcc-multilib）"
echo "     2. 在脚本当前目录及\${OSLAB_PATH}/bochs/下生成一个bochs-gdb（若没有生成,使用./setup.sh -d 重新执行脚本）并执行sudo make install将bochs可执行文件安装进/usr/local/bin/目录下等安装操作（doc，share，man等等）"

env_install
bochs_install $1

