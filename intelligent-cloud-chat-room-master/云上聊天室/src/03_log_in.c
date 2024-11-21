#include "03_log_in.h"
#include "04_Sqlite3_log_in.h"
#include <sys_head.h>

// 登陆
/*
功能 通过传入 的用户名和密码 去数据库中进行比对如果有
成功
    返回 true
    失败 false
*/

bool log_in(sqlite3 *db, char *name, char *password)
{
    // 验证账号密码的安全性
    if (name == NULL || password == NULL)
    {
        printf("输入错误\n");
        return false;
    }

    // 通过名字查找 密码
    char *sqlite3_password = Select_FROM_log_in_passwd(db, name);

    if (NULL == sqlite3_password)
    {
        printf("无用户,请先注册\n");
        return false;
    }

    if (!strcmp(sqlite3_password, password))
    {
        printf("密码正确\n");

        free(sqlite3_password);
        return true;
    }
    else
    {
        free(sqlite3_password);
        printf("密码错误\n");
        return false;
    }
}

// 注册
/*
功能 通过传入 的用户名和密码 去数据库中进行插入操作 (不能有重名用户)
成功
    返回 true
    失败 false
*/

bool Register(sqlite3 *db, char *name, char *password)
{
    // 验证账号密码的安全性
    if (name == NULL || password == NULL)
    {
        printf("输入错误\n");
        return false;
    }
    // 验证账号重名问题
    if (NULL != Select_FROM_log_in_passwd(db, name))
    {
        printf("已有账号\n");
        return false;
    }
    else
    {
        Insert_INTO_log_in(db, name, password,"log_in");
        printf("注册成功\n");
        sqlite3_creat_table(db,name);//创建专属数据库
        return true;
    }
}

