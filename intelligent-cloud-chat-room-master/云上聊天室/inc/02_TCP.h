#ifndef __02_TCP_H__
#define __02_TCP_H__

#include <sys_head.h>
#include "03_log_in.h"
#include "05_Arg.h"
#include "04_Sqlite3_log_in.h"

// 定义一个全局变量的数组
typedef struct tpc_Sock_fds
{
    int Sock_fd;
    char name[128];
    sqlite3 *db;
} tpc_Sock_fds;

#define MAX_LIST 20
// 全局变量 顺序表 存储用户数据
tpc_Sock_fds sock_fds_User[MAX_LIST];

// 初始化TCP通讯
int TCP_init(int potr);

// 客户端连接
int TCP_accept(int server_fd);

// 判断登录 注册
int Judge_User(sqlite3 *db, int Acce_fd, int index);

// 群聊
int Chatting(int index, int Sock_fd, char *name);

//发文件
void File_send(int Sock_fd);

//添加好友
void Add_Friends(sqlite3 *db,int Sock_fd,char *table);
//好友私聊
void Private_message(sqlite3 *db,int Sock_fd ,int Friend_fd , char *table);
//删除好友
void Delect_Friends(sqlite3 *db,int Sock_fd,char *table);
// 用户操作界面
void *Tcp_server_func(void *arg);

#endif