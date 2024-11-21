#include "02_TCP.h"

// 定义互斥锁
pthread_mutex_t lock;
// 全局变量 顺序表 存储用户数据
tpc_Sock_fds sock_fds_chat[MAX_LIST];
int arr_len_chat = 0;
// 初始化函数

int TCP_init(int port)
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 配置地址结构
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // 绑定套接字
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 监听套接字
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("服务器启动成功 端口: %d...\n", port);

    return server_fd;
}

int TCP_accept(int server_fd)
{
    int new_socket;
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);

    // 阻塞等待客户端连接
    if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("新连接: socket fd = %d, ip : %s, port: %d\n", new_socket,
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    return new_socket;
}

// 判断登录 注册
int Judge_User(sqlite3 *db, int Acce_fd, int index)
{
    // 声明变量
    char name[64];
    char passwd[64];
    char buf[128];
    char buf_init[1024] = "欢迎链接\n登录(log_in)\n注册(register)\n请选择输入对应的英文\n";
    // 初始化数据
    memset(name, 0, sizeof(name));
    memset(passwd, 0, sizeof(passwd));
    memset(buf, 0, sizeof(buf));
    // 输入数据
    send(Acce_fd, buf_init, strlen(buf_init), 0);
    recv(Acce_fd, buf, sizeof(buf), 0);
    send(Acce_fd, "请输入用户名/密码\n", strlen("请输入用户名/密码\n"), 0);
    recv(Acce_fd, name, sizeof(name), 0);
    recv(Acce_fd, passwd, sizeof(passwd), 0);
    // 注册
    if (!strcmp(buf, "register"))
    {
        bool B = Register(db, name, passwd);
        if (B == true)
        {
            send(Acce_fd, "注册成功\n", strlen("注册成功\n"), 0);
            return -1;
        }
        else
        {
            send(Acce_fd, "注册失败：重名，请换一个昵称\n", strlen("注册失败：重名，请换一个昵称\n"), 0);
            send(Acce_fd, buf_init, strlen(buf_init), 0);
            return -1;
        }
    }
    else if (!strcmp(buf, "log_in"))
    {
        // 登录
        bool L = log_in(db, name, passwd);
        if (L == true)
        {
            send(Acce_fd, "登录成功\n", strlen("登录成功\n"), 0);
            strcpy(sock_fds_User[index].name, name);
        }
        else
        {
            send(Acce_fd, "密码错误，请重新登录\n", strlen("密码错误，请重新登录\n"), 0);
            return -1;
        }
    }
    return 0;
}

// 群聊
int Chatting(int index, int Sock_fd, char *name)
{
    // 声明变量
    char buf_init[128];
    char buf[1024];
    char send_buf[1024];
    // 初始化数据
    strcpy(sock_fds_chat[index].name, name);
    sprintf(buf_init, "%s:%s", sock_fds_chat[index].name, "进入群聊");

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        memset(send_buf, 0, sizeof(send_buf));
        recv(Sock_fd, buf, sizeof(buf), 0);
        pthread_mutex_lock(&lock);

        // 修改名字
        if (NULL != strstr(buf, "set_name_"))
        {
            // 写入名字
            strcpy(sock_fds_chat[index].name, buf + 9);
            sprintf(buf, "修改昵称成功：%s", sock_fds_chat[index].name);
            send(Sock_fd, buf, strlen(buf), 0);
        }
        // 退出聊天
        else if (!strcmp(buf, "quit"))
        {
            sprintf(send_buf, "%s:%s", sock_fds_chat[index].name, "退出成功");
            send(Sock_fd, send_buf, strlen(send_buf), 0);
            pthread_mutex_unlock(&lock);
            break;
        }
        // 转发 群发
        else
        {
            // 发送给其他成员
            for (size_t i = 0; i < MAX_LIST; i++)
            {
                sprintf(send_buf, "[%s]:%s", sock_fds_chat[index].name, buf);
                send(sock_fds_chat[i].Sock_fd, buf_init, strlen(buf_init), 0);
                memset(buf_init, 0, sizeof(buf_init));
                if (Sock_fd != sock_fds_chat[i].Sock_fd)
                {
                    send(sock_fds_chat[i].Sock_fd, send_buf, strlen(send_buf), 0);
                }
            }
        }
        pthread_mutex_unlock(&lock);
    }

    // 从顺序表中删除
    pthread_mutex_lock(&lock);
    for (size_t i = index; i < MAX_LIST; i++)
    {
        sock_fds_chat[i] = sock_fds_chat[i + 1];
    }
    arr_len_chat--;
    printf("群聊已退出\n");
    pthread_mutex_unlock(&lock);
    return 0;
}

// 发文件
void File_send(int Sock_fd)
{
    // 声明变量
    agr info;
    char file[128];
    char buf[1024];
    // 初始化变量
    memset(buf, 0, sizeof(buf));
    memset(file, 0, sizeof(file));
    info.request.bytes = 1;
    send(Sock_fd, "请输入需发送的文件夹", strlen("请输入需发送的文件夹"), 0);
    recv(Sock_fd, file, sizeof(file), 0);
    // 1服务器 回传 文件名
    DIR *dir_ = opendir("data");
    struct dirent *entry;
    info.File_list.num = 0;
    while ((entry = readdir(dir_)) != NULL)
    {
        sprintf(buf, "%s", entry->d_name);
        // 不发送隐藏文件
        if ('.' == buf[0])
        {
            continue;
        }

        strcat(info.File_list.data, " | ");
        strcat(info.File_list.data, buf);
        info.File_list.num++;

        // 到达文件名最大存储
        if (7 == info.File_list.num)
        {
            break;
        }
    }

    info.File_list.bytes = 2;

    send(Sock_fd, info.buf, strlen(info.buf), 0);
    // 2客户端 发送 文件名
    memset(file, 0, sizeof(file));
    recv(Sock_fd, file, sizeof(file), 0);
    // 3服务器 发送 文件
    // 打开文件
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s%s", "./data/", file);
    // 查看文件名
    printf("%s\n", buf);
    int fd = open(buf, O_RDONLY);
    if (-1 == fd)
    {
        printf("文件打开失败\n");
        return;
    }

    lseek(fd, 0, SEEK_SET);
    // 发送文件
    while (0 != read(fd, info.File_data.data, sizeof(info.File_data.data) - 1))
    {
        info.File_data.bytes = 4;
        write(Sock_fd, info.buf, strlen(info.buf));
        memset(info.buf, 0, sizeof(info.buf));
    }
}

// 添加好友
void Add_Friends(sqlite3 *db, int Sock_fd, char *table)
{
    char name[128];
    char buf_send[1024];
    char buf_recv[1024];
    memset(name, 0, sizeof(name));
    memset(buf_send, 0, sizeof(buf_send));
    memset(buf_recv, 0, sizeof(buf_recv));
    send(Sock_fd, "请输入要添加的好友昵称\n", strlen("请输入要添加的好友昵称\n"), 0);
    recv(Sock_fd, name, sizeof(name), 0);
    printf("我的%d", Sock_fd);
    for (size_t i = 0; i < MAX_LIST; i++)
    {
        if (!strcmp(sock_fds_User[i].name, name))
        {
            printf("好友的%d", sock_fds_User[i].Sock_fd);
            sprintf(buf_send, "%s%s", name, "请求添加你为好友，是否同意(yes/no)");
            send(sock_fds_User[i].Sock_fd, buf_send, strlen(buf_send), 0);
            int id_f = sqlite3_Select_User_log_in(db, name, "log_in");
            int id_m = sqlite3_Select_User_log_in(db, table, "log_in");
            Insert_INTO_Table(db, id_f, name, "on", table); // 此时的password 就是online
            Insert_INTO_Table(db, id_m, table, "on", name); // 此时的password 就是online
            send(Sock_fd, "添加好友成功\n", strlen("添加好友成功\n"), 0);
        }
    }
}

// 删除好友
void Delect_Friends(sqlite3 *db, int Sock_fd, char *table)
{
    int id;
    send(Sock_fd, "请输入要删除的好友id\n", strlen("请输入要删除的好友id\n"), 0);
    recv(Sock_fd, &id, sizeof(id), 0);
    sqlite3_Delete_FROM_log_in(db, id, table);
}

// 好友私聊
void Private_message(sqlite3 *db, int Sock_fd, int Friend_fd, char *name)
{
    char buf[1024];
    char send_buf[1024];
    send(Sock_fd,"开始聊天\n",strlen("开始聊天\n"),0);
    while (1)
    {
        memset(buf,0,sizeof(buf));
        recv(Sock_fd, buf, sizeof(buf), 0);
        pthread_mutex_lock(&lock);
        //退出聊天
        if (!strcmp(buf, "quit"))
        {
            send(Sock_fd, "聊天结束", strlen("聊天结束"), 0);
            pthread_mutex_unlock(&lock);
            break;
        }
        else
        {
            //发给对方
            sprintf(send_buf, "[%s]:%s", name, buf);
            send(Friend_fd, send_buf, strlen(send_buf), 0);
        }
        pthread_mutex_unlock(&lock);
    }
}

// 用户操作界面
void *Tcp_server_func(void *arg)
{
    // 声明变量
    int index = *((int *)arg); // 用户顺序表
    int Sock_fd;
    sqlite3 *db;
    char Order[128];
    char name[64];
    char buf[1024];
    int F = 0;
    int in_chat = 0; // 群聊顺序表
    char name_friend[64];
    int Friend_fd = 0;

    // 初始化数据
    Sock_fd = sock_fds_User[index].Sock_fd;
    db = sock_fds_User[index].db;
    // 初始化互斥锁
    pthread_mutex_init(&lock, NULL);

// 登录注册
Q1:
    F = Judge_User(db, Sock_fd, index);
    if (F == -1)
    {
        goto Q1;
    }
    while (1)
    {
        memset(Order, 0, sizeof(Order));
        memset(name_friend, 0, sizeof(name_friend));
        memset(buf, 0, sizeof(buf));
        strcpy(name, sock_fds_User[index].name);
        strcpy(buf, "加入群聊(join_in_chat)\n发送文件(file_download)\n添加好友(Add_Friends)\n好友私聊(Private_message)\n删除好友(Delect_Friends)\n查看好友列表(Friends_list)");
        send(Sock_fd, buf, strlen(buf), 0);
        recv(Sock_fd, Order, sizeof(Order), 0);
        // 群聊
        if (!strcmp(Order, "join_in_chat"))
        {
            pthread_mutex_lock(&lock);
            sock_fds_chat[arr_len_chat].Sock_fd = Sock_fd;
            strcpy(sock_fds_chat[arr_len_chat].name, name);
            in_chat = arr_len_chat;
            pthread_mutex_unlock(&lock);
            arr_len_chat++;
            Chatting(in_chat, Sock_fd, name);
        }

        // 发文件
        if (!strcmp(Order, "file_download"))
        {
            File_send(Sock_fd);
        }

        // 添加好友
        if (!strcmp(Order, "Add_Friends"))
        {
            Add_Friends(db, Sock_fd, name);
        }
        // 查看好友列表
        if (!strcmp(Order, "Friends_list"))
        {
            send(Sock_fd,"ID\t用户名\t是否在线\n",strlen("ID\t用户名\t是否在线\n"),0);
            sqlite3_printf_log_in(db, Sock_fd, name);
            send(Sock_fd,"\n-------------------------------------------------\n",strlen("\n-------------------------------------------------\n"),0);
        }
        // 删除好友
        if (!strcmp(Order, "Delect_Friends"))
        {
            Delect_Friends(db, Sock_fd, name);
        }
        // 好友私聊
        if (!strcmp(Order, "Private_message"))
        {
            send(Sock_fd, "请选择好友进行私聊\n", strlen("请选择好友进行私聊\n"), 0);
            recv(Sock_fd, name_friend, sizeof(name_friend), 0);
            for (size_t i = 0; i < MAX_LIST; i++)
            {
                if (!strcmp(sock_fds_User[i].name, name_friend))
                {
                    Friend_fd = sock_fds_User[i].Sock_fd;
                    break;
                }
            }
            sprintf(buf, "%s%s", name, "请求与你单聊,如若同意,请输入agree Private_message");
            send(Friend_fd, buf, strlen(buf), 0);
            Private_message(db, Sock_fd, Friend_fd, name);
        }
        if (NULL != strstr(Order, "agree Private_message"))
        {
            send(Sock_fd, "请再次输入对方姓名\n", strlen("请再次输入对方姓名\n"), 0);
            recv(Sock_fd, name_friend, sizeof(name_friend), 0);
            if (!strcmp(name_friend, "quit"))
            {
                continue;
            }
            for (size_t i = 0; i < MAX_LIST; i++)
            {
                if (!strcmp(sock_fds_User[i].name, name_friend))
                {
                    Friend_fd = sock_fds_User[i].Sock_fd;
                    break;
                }
            }
            Private_message(db, Sock_fd, Friend_fd, name);
        }

        if (!strcmp(Order, "quit"))
        {
            break;
        }
    }

    return NULL;
}
