#include "01_main.h"


// 定义互斥锁
pthread_mutex_t lock;
int arr_len_User = 0;


int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("格式错误：%s port\n", argv[0]);
        return -1;
    }
    // 初始化TCP 服务器
    int Sock_fd = TCP_init(atoi(argv[1]));

    // 初始化数据库

    sqlite3 *db = sqlite3_init_log_in();

    // 初始化互斥锁
    pthread_mutex_init(&lock, NULL);
    pthread_t Tid;
    int index = 0;

    while (1)
    {
        // 等待客户端连接
        int Acce_fd = TCP_accept(Sock_fd);
        //更新用户顺序表
        pthread_mutex_lock(&lock);
        sock_fds_User[arr_len_User].Sock_fd = Acce_fd;
        sock_fds_User[arr_len_User].db = db;
        index = arr_len_User;
        pthread_mutex_unlock(&lock);
        pthread_create(&Tid, NULL, Tcp_server_func, (void *)&index);
        // 线程分离
        pthread_detach(Tid); // 不阻塞
        arr_len_User++;
        printf("链接的用户：%d",arr_len_User);
    }
    close(Sock_fd);
    return 0;
}