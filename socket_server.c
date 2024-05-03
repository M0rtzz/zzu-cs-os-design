/**
 * @file socket_server.c
 * @brief  服务器
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
#define MAX_LINK_NUM 5 // 最大连接数

// 用已连接客户端套接字
int client_sockfd[MAX_LINK_NUM];
// 服务器套接字
int server_sockfd;
// 当前连接数
int cur_link = 0;
// 同步服务器连接数的信号量
sem_t mutex;
// 答案
int secret_num = 0;

/**
 * @brief 服务器与客户端的收发通信函数，n为连接数组序号
 * @param n
 */
void rcv_snd(int n)
{
    int retval;
    char recv_buf[1024];
    char send_buf[1024];
    pthread_t tid;
    tid = pthread_self();

    printf("服务器线程id=%lu使用套接字%d与客户机对话开始...\n", tid, client_sockfd[n]);

    // 发送随机数给客户端
    // sprintf(send_buf, "答案是%d", secret_number);
    // write(client_sockfd[n], send_buf, strlen(send_buf));

    do
    {
        memset(recv_buf, 0, 1024);
        int rcv_num = read(client_sockfd[n], recv_buf, sizeof(recv_buf));
        if (rcv_num > 0)
        {
            int guess = atoi(recv_buf); // 将客户端的输入转换为整数

            if (guess < secret_num)
            {
                strcpy(send_buf, "小了");
            }
            else if (guess > secret_num)
            {
                strcpy(send_buf, "大了");
            }
            else
            {
                strcpy(send_buf, "猜对了");
                write(client_sockfd[n], send_buf, strlen(send_buf));
                break;
            }

            write(client_sockfd[n], send_buf, strlen(send_buf));
        }
    } while (true);

    printf("服务器线程id=%lu与客户机对话结束\n", tid);
    close(client_sockfd[n]);
    // 关闭服务器监听套接字
    close(server_sockfd);
    client_sockfd[n] = -1;
    cur_link--;
    printf("当前连接数为：%d\n", cur_link);
    sem_post(&mutex);
    pthread_exit(&retval);
}

signed main()
{
    // 初始化随机数生成器
    srand(time(NULL));

    // 生成1到100的随机数
    secret_num = rand() % 100 + 1;

    printf("答案是：%d\n", secret_num);

    socklen_t client_len = 0;
    // 服务器端协议地址
    struct sockaddr_in server_addr;
    // 客户端协议地址
    struct sockaddr_in client_addr;
    // 连接套接字数组循环变量
    int i = 0;
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 指定网络套接字
    server_addr.sin_family = AF_INET;
    // 接受所有IP地址的连接
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 绑定到9736端口
    server_addr.sin_port = htons(9736);
    bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // 协议套接字命名为server_sockfd
    printf("1、服务器开始listen...\n");
    // 创建连接数最大为MAX_LINK_NUM的套接字队列，监听命名套接字，listen不会阻塞，它向内核报告套接字和最大连接数
    listen(server_sockfd, MAX_LINK_NUM);
    // 忽略子进程停止或退出信号
    signal(SIGCHLD, SIG_IGN);

    for (i = 0; i < MAX_LINK_NUM; i++)
        client_sockfd[i] = -1; // 初始化连接队列

    sem_init(&mutex, 0, MAX_LINK_NUM); // 信号量mutex初始化为连接数

    while (true)
    {
        // 搜寻空闲连接
        for (i = 0; i < MAX_LINK_NUM; i++)
            if (client_sockfd[i] == -1)
                break;

        // 如果达到最大连接数，则客户等待
        if (i == MAX_LINK_NUM)
        {
            printf("已经达到最大连接数%d,请等待其它客户释放连接...\n", MAX_LINK_NUM);
            // 阻塞等待空闲连接
            sem_wait(&mutex);
            // 被唤醒后继续监测是否有空闲连接
            continue;
        }

        client_len = sizeof(client_addr);
        printf("2、服务器开始accept...i=%d\n", i);
        client_sockfd[i] = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
        // 当前连接数增1
        cur_link++;
        // 可用连接数信号量mutex减1
        sem_wait(&mutex);
        printf("当前连接数为：%d(<=%d)\n", cur_link, MAX_LINK_NUM);
        // 输出客户端地址信息
        printf("连接来自:连接套接字号=%d,IP地址=%s,端口号=%d\n", client_sockfd[i], inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        pthread_create(malloc(sizeof(pthread_t)), NULL, (void *)(&rcv_snd), (void *)i);
    }

    return EXIT_SUCCESS;
}