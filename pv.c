/**
 * @file pv.c
 * @brief 生产者、计算者、消费者问题
 * @author M0rtzz E-mail : m0rtzz@outlook.com
 * @version 1.0
 * @date 2024-04-30
 *
 */

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define THREAD_NUM 3

/**
 * @brief 两缓冲区
 */
char buffer_1[BUFFER_SIZE];
char buffer_2[BUFFER_SIZE];

/**
 * @brief empty_i 表示 buffer_i 中空闲位置信号量，full_i 表示 buffer_i 中填充数据信号量
 */
sem_t empty_1, full_1, empty_2, full_2;

/**
 * @brief 生产者
 * @param arg
 * @return void*
 */
void *producerFunc(void *arg)
{
    while (true)
    {
        // 初始化随机数种子
        srand((unsigned int)time(NULL));
        // 生成随机小写字母
        char item = 'a' + rand() % 26;

        // P
        sem_wait(&empty_1);
        // 放入 buffer_1
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (buffer_1[i] == '\0')
            {
                buffer_1[i] = item;
                break;
            }
        }

        // V
        sem_post(&full_1);

        sleep(1);
    }

    return NULL;
}

/**
 * @brief 计算者
 * @param arg
 * @return void*
 */
void *calculatorFunc(void *arg)
{
    while (true)
    {
        // P
        sem_wait(&full_1);
        // 从 buffer_1 中取出字母，转换为大写字母后放入 buffer_2
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (buffer_1[i] != '\0')
            {
                buffer_2[i] = toupper(buffer_1[i]);
                buffer_1[i] = '\0';
                break;
            }
        }

        // V
        sem_post(&empty_1);
        sem_post(&full_2);

        sleep(1);
    }

    return NULL;
}

/**
 * @brief 消费者
 * @param arg
 * @return void*
 */
void *consumerFunc(void *arg)
{
    while (true)
    {
        // P
        sem_wait(&full_2);
        // 从 buffer_2 中取出字符并打印到屏幕上
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (buffer_2[i] != '\0')
            {
                printf("%c\n", buffer_2[i]);
                buffer_2[i] = '\0';
                break;
            }
        }

        // V
        sem_post(&empty_2);

        sleep(1);
    }

    return NULL;
}

signed main()
{
    pthread_t threads[THREAD_NUM];

    // 初始化信号量
    sem_init(&empty_1, 0, BUFFER_SIZE);
    sem_init(&full_1, 0, 0);
    sem_init(&empty_2, 0, BUFFER_SIZE);
    sem_init(&full_2, 0, 0);

    // 创建线程
    pthread_create(&threads[0], NULL, producerFunc, NULL);
    pthread_create(&threads[1], NULL, calculatorFunc, NULL);
    pthread_create(&threads[2], NULL, consumerFunc, NULL);

    // 等待线程结束
    for (int i = 0; i < THREAD_NUM; ++i)
        pthread_join(threads[i], NULL);

    // 销毁信号量
    sem_destroy(&empty_1);
    sem_destroy(&full_1);
    sem_destroy(&empty_2);
    sem_destroy(&full_2);

    return EXIT_SUCCESS;
}
