#!/bin/zsh

# ------------------------------------------------------------------
# @file: umount.sh
# @brief: 杀死全部占用./hdc/的进程并卸载文件系统
# @author: M0rtzz
# @date: 2024-05-01
# ------------------------------------------------------------------

hdc_path=$(realpath ./hdc)

# 循环杀死进程直到没有进程占用
while true; do
    pid_array=()

    while IFS= read -r pid; do
        pid_array+=("${pid}")
    done < <(lsof "${hdc_path}" | awk '$2 != "PID" {print $2}')

    if [ ${#pid_array[@]} -eq 0 ]; then
        if sudo umount "${hdc_path}" >/dev/null 2>&1; then
            echo -e "\033[1;34m没有进程占用./hdc\033[0m"
            echo -e "\033[1;32m成功卸载文件系统\033[0m"
            break
        else
            echo -e "\033[1;31m无法卸载文件系统，正在重试...\033[0m"
        fi
    else
        echo -e "\033[1;36m已杀死占用./hdc的进程，PID为：\033[0m${pid_array[*]}"
        sudo kill -9 "${pid_array[@]}"
    fi
    sleep 1
done
