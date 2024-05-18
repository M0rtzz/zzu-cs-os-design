#!/usr/bin/sudo /bin/zsh

# 如果在编译 bochs 时遇到 libgtk2.0-dev 依赖问题，请手动执行：
# sudo apt update && sudo apt install aptitude && sudo aptitude install libgtk2.0-dev

function _echoInfo() {
    echo -e "[Info]: ${1}"
}

function _echoError() {
    echo -e "\033[31m[Error]: ${1} \033[0m"
}

function _echoSuccess() {
    echo -e "\033[32m[Success]: ${1} \033[0m"
}

function envInstall() {

    if sudo apt install -y make bin86 gcc-multilib &>/dev/null; then
        _echoSuccess "bin86 is installed."
    else
        _echoError "bin86 is not installed!!!"
    fi

    if [ -n "$(command -v gcc-3.4)" ]; then
        _echoSuccess "GCC-3.4 is installed."
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

    if [ -z "$(command -v gcc-3.4)" ]; then
        _echoInfo "Start installing gcc-3.4..."

        for deb in ${DOWNLOAD_LIST[*]}; do
            if [ ! -e ${GCC_DIR}/"${deb}" ]; then
                # wget http://old-releases.ubuntu.com/ubuntu/pool/universe/g/gcc-3.4/${deb} -P ${GCC_DIR} -q --show-progress && \
                # NOTE: 替换为中科大镜像站
                if wget http://mirrors.ustc.edu.cn/ubuntu-old-releases/ubuntu/pool/universe/g/gcc-3.4/"${deb}" -P "${GCC_DIR}" -q --show-progress; then
                    _echoInfo "Download ${deb} Successfully."
                else
                    rm "${deb}" &
                    _echoError "Download ${deb} Unsuccessfully!!!"
                fi
            fi
        done

        sudo dpkg -i ${GCC_DIR}/*.deb &>/dev/null
        sudo apt install -y -f &>/dev/null
        if [ -n "$(command -v gcc-3.4)" ]; then
            _echoSuccess "gcc-3.4 is installed."
        fi
        rm -rf ${GCC_DIR}
    fi
}

function bochsInstall() {

    sudo apt install -y build-essential libgtk2.0-dev \
        libx11-dev xserver-xorg-dev xorg-dev g++ \
        pkg-config libxcursor-dev libxrandr-dev \
        libxinerama-dev libxi-dev &>/dev/null

    # INFO: https://github.com/oldlinux-web/oldlinux-files/blob/master/bochs/README_FIRST
    # NOTE: M0rtzz have resolved the bug in the source code
    # if [ ! -e "bochs-2.2.5.tar.gz" ]; then
    #     # wget https://downloads.sourceforge.net/project/bochs/bochs/2.6.9/bochs-2.6.9.tar.gz -q --show-progress  && \
    #     if wget https://sourceforge.net/projects/bochs/files/bochs/2.2.5/bochs-2.2.5.tar.gz -q --show-progress; then
    #         _echoSuccess "Download bochs-2.2.5.tar.gz Successfully."
    #     else
    #         rm bochs-2.2.5.tar.gz
    #         _echoError "Download bochs-2.2.5.tar.gz unsuccessfully!!!"
    #     fi
    # fi

    # if [ ! -d "bochs-2.2.5" ]; then
    #     if tar zxvf bochs-2.2.5.tar.gz &>/dev/null; then
    #         _echoInfo "tar bochs-2.2.5.tar.gz Successfully."
    #     else
    #         rm -rf ../bochs-2.2.5
    #         _echoError "tar bochs-2.2.5.tar.gz unsuccessfully!!!"
    #     fi

    # fi

    if [ -d "/usr/local/share/bochs/" ]; then
        sudo rm -rf "/usr/local/share/bochs/"
    fi

    if [ -d "bochs-2.2.5" ]; then
        cd bochs-2.2.5 || exit
        make clean
        # 添加 `-fpermissive` 以防编译报错
        sed -i 's/CFLAGS="-g -O2"/CFLAGS="-g -O2 -fpermissive"/g' ./configure
        sed -i 's/CFLAGS="-g"/CFLAGS="-g -fpermissive"/g' ./configure
        sed -i 's/CFLAGS="-O2"/CFLAGS="-O2 -fpermissive"/g' ./configure
        sed -i 's/CXXFLAGS="-g -O2"/CXXFLAGS="-g -O2 -fpermissive"/g' ./configure
        sed -i 's/CXXFLAGS="-g"/CXXFLAGS="-g -fpermissive"/g' ./configure
        sed -i 's/CXXFLAGS="-O2"/CXXFLAGS="-O2 -fpermissive"/g' ./configure

        # sudo apt install aptitude && sudo apt install libgtk2.0-dev
        sudo apt update && sudo apt install libgtk2.0-dev

        # TAG: 支持GNU gdb
        # ./configure --enable-gdb-stub --enable-disasm
        ./configure --enable-gdb-stub --enable-new-pit --enable-all-optimizations --enable-4meg-pages --enable-global-pages --enable-pae --enable-sep --enable-cpu-level=6 --enable-sse=2 --enable-show-ips --disable-reset-on-triple-fault --with-all-libs

        # TAG: 包含bochs自带的debug工具
        # ./configure --enable-debugger --enable-disasm
        # ./configure --enable-disasm --enable-debugger --enable-new-pit --enable-all-optimizations --enable-4meg-pages --enable-global-pages --enable-pae --enable-sep --enable-cpu-level=6 --enable-sse=2 --disable-reset-on-triple-fault --with-all-libs

        if make -j"$(nproc)" && sudo chown -R "$(who | awk '{print $1}')" ../bochs-2.2.5 && (
            cp bochs ./bochsdbg &&
                cp bochs ../../../tools/bochs/bochs-gdb &&
                sudo chown -R "$(who | awk '{print $1}')" ../../../tools/bochs/bochs-gdb
        ); then
            sudo make install
            _echoSuccess "make bochs successfully."
        else
            _echoError "make bochs unsuccessfully.!!!"
        fi
    fi
}

trap 'onCtrlC' INT

function onCtrlC() {
    _echoError "[Warning]: stopped by user."
    rm -rf ${GCC_DIR}
    exit
}

echo "须知"
echo "脚本将完成以下两件事："
echo "    1. 为系统安装相应的编译环境（make，bin86，gcc-3.4，gcc-multilib）"
echo "    2. 在\${TOOLS_PATH}/bochs/下生成一个bochs-gdb，然后使用sudo make install将bochs可执行文件安装到/usr/local/bin/目录中，同时执行其他安装操作（例如install_doc、install_share、install_man等）"

envInstall
bochsInstall
