/**
 * @file m0rtzz.c
 * @brief 实现自定义的系统调用函数
 * @author M0rtzz
 * @version 1.0
 * @date 2024-04-30
 */

#include <errno.h>
#include <string.h>
#include <asm/segment.h>

char msg[30]; // 全局变量，用于存储用户传递的消息

/**
 * @brief 实现 `sys_m0rtzz` 系统调用函数，将用户提供的字符串拷贝到内核空间
 * @param str 用户提供的字符串指针
 * @return 返回拷贝的字符个数，如果超过30个字符，则返回负值错误码
 */
int sys_m0rtzz(const char *str)
{
    int i;
    char tmp[40]; // 临时缓冲区，用于存储从用户空间读取的字符串

    // 从用户空间逐个字符读取，直到遇到结束符或者缓冲区满为止
    for (i = 0; i < 40; i++)
    {
        // 从用户空间读取一个字符并存储到临时缓冲区中
        tmp[i] = get_fs_byte(str + i);

        if (tmp[i] == '\0') // 如果读取到字符串结束符，则退出循环
            break;
    }

    // 统计读取的字符个数
    i = 0;
    while (i < 40 && tmp[i] != '\0')
        i++;

    int len = i;

    // 如果读取的字符个数超过30个，则返回错误码
    if (len > 30)
        return -(EINVAL);

    // 将读取的字符串拷贝到全局变量msg中
    strcpy(msg, tmp);

    // 返回拷贝的字符个数
    return i;
}

/**
 * @brief 实现 `sys_ashore` 系统调用函数，将内核空间中的消息拷贝到用户提供的缓冲区中
 * @param str 用户提供的缓冲区指针，用于存储消息
 * @param size 缓冲区的大小
 * @return 返回拷贝的字符个数，如果缓冲区大小不足，则返回负值错误码
 */
int sys_ashore(char *str, unsigned int size)
{
    int len = 0;

    // 统计全局变量msg中的字符个数
    for (; msg[len] != '\0'; len++)
        ;

    // 如果全局变量msg中的字符个数超过了缓冲区的大小，则返回错误码
    if (len > size)
        return -(EINVAL);

    // 将全局变量msg中的消息拷贝到用户提供的缓冲区中
    int i;
    for (i = 0; i < size; i++)
    {
        put_fs_byte(msg[i], str + i); // 将字符逐个写入用户空间

        if (msg[i] == '\0') // 如果遇到字符串结束符，则退出循环
            break;
    }

    // 返回拷贝的字符个数
    return i;
}