/**
 * @file socket_client.c
 * @brief  客户机
 * @author M0rtzz E-mail : m0rtzz@outlook.com
 * @version 1.0
 * @date 2024-05-01
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

signed main()
{
    // 客户端套接字描述符
    int sockfd;
    int len = 0;
    struct sockaddr_in address; // 套接字协议地址
    // 发送消息缓冲区
    char snd_buf[1024];
    // 接收消息缓冲区
    char rcv_buf[1024];
    int result;
    // 接收消息长度
    int rcv_num;
    // 客户进程标识符
    pid_t cpid;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("客户端创建套接字失败！\n");
        return EXIT_FAILURE;
    }

    // 使用网络套接字
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器地址
    // 服务器所监听的端口
    address.sin_port = htons(9736);

    if (inet_aton("127.0.0.1", &address.sin_addr) < 0)
    {
        printf("inet_aton error.\n");
        return -EXIT_FAILURE;
    }

    len = sizeof(address);
    // 获取客户进程标识符
    cpid = getpid();
    printf("1、客户机%d开始connect服务器...\n", cpid);
    result = connect(sockfd, (struct sockaddr *)&address, len);

    if (result == -1)
    {
        perror("客户机connect服务器失败!\n");
        exit(EXIT_FAILURE);
    }

    printf("-----------客户机%d与服务器线程对话开始...\n", cpid);

    // 客户机与服务器循环发送接收消息
    do
    {

        printf("2.客户机%d--->服务器:sockfd=%d,请输入客户机要发送给服务器的消息：", cpid, sockfd);
        // 发送缓冲区清零
        memset(snd_buf, 0, 1024);
        scanf("%s", snd_buf); // 键盘输入欲发送给服务器的消息字符串
        // 将消息发送到套接字
        write(sockfd, snd_buf, sizeof(snd_buf));

        if (strncmp(snd_buf, "quit", 2) == 0)
            break; // 若发送"quit"，则结束循环，通信结束

        // 接收缓冲区清零
        memset(rcv_buf, 0, 1024);
        printf("客户机%d,sockfd=%d 等待服务器回应...\n", cpid, sockfd);
        rcv_num = read(sockfd, rcv_buf, sizeof(rcv_buf));
        printf("客户机%d,sockfd=%d 从服务器接收的消息长度=%lu\n", cpid, sockfd, strlen(rcv_buf));
        // 输出客户机从服务器接收的消息
        printf("3.客户机%d<---服务器:sockfd=%d,客户机从服务器接收到的消息是：%s\n", cpid, sockfd, rcv_buf);

        sleep(1);

    } while (strncmp(rcv_buf, "猜对了", 2) != 0); // 如果收到"!q"，则结束循环，通信结束
    printf("-----------客户机%d,sockfd=%d 与服务器线程对话结束---------\n", cpid, sockfd);
    // 关闭客户机套接字
    close(sockfd);

    return EXIT_SUCCESS;
}