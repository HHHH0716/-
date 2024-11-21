#include "04_Sqlite3_log_in.h"
#include <sys_head.h>

// 回调函数
int find_User_Password(void *Password, int argc, char **argv, char **azColName)
{
    if (argc > 0 && argv[0] != NULL)
    {
        // 检查密码长度是否超出缓冲区大小
        if (strlen(argv[0]) < 64)
        {
            strcpy((char *)Password, argv[0]);
            printf("Password: %s\n", (char *)Password);
        }
        else
        {
            printf("Password too long!\n");
        }
    }
    else
    {
        printf("No password found.\n");
        strcpy((char *)Password, ""); // 或者处理没有结果的情况
    }
    return 0;
}

// 回调函数
int find_User_id(void *data, int argc, char **argv, char **azColName)
{
    printf("name %s\n" , argv[0]);
    strcpy( (char *)data , argv[0]);
    return 0;
}

int callback_name(void *data, int argc, char **argv, char **azColName)
{
    int *id = (int *)data;
    *id = atoi(argv[0]);
    printf("id = %d \n", *id);
    return 0;
}

int callback_prient(void *data, int argc, char **argv, char **azColName)
{
    int * Sock_fd = (int *)data;
    char buf[128];
    char cat[1024];
    memset(buf,0,sizeof(buf));
    memset(cat,0,sizeof(cat));
    for (int i = 0; i < argc; ++i)
    {
        sprintf(buf,"%s\t", (argv[i] ? argv[i] : "NULL"));
        strcat(cat,buf);
    }
    strcat(cat,"\n");
    send(*Sock_fd,cat,strlen(cat),0);
    return 0;
}


sqlite3 *sqlite3_init_log_in(void)
{
    // 打开数据库
    int sqlite_Errnum = 0;
    sqlite3 *sqlite3_log_in;
    char *errmsg;
    sqlite_Errnum = sqlite3_open("./data/log_in.db", &sqlite3_log_in);
    if (SQLITE_OK != sqlite_Errnum)
    {
        printf("sqlite3_open : errnum:[%d] errstr:[%s]\n", sqlite_Errnum, sqlite3_errmsg(sqlite3_log_in));
        exit(-1);
    }

    printf("打开成功\n");

    // 建立表

    char sql_cmd[128];

    sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s (id INTEGER PRIMARY KEY AUTOINCREMENT , User CHAR(64), password CHAR(64))", Tab_name);
    // INTEGER PRIMARY KEY AUTOINCREMENT 从 1 开始自增
    // IF NOT EXISTS 当有表格时 不会报错
    sqlite_Errnum = sqlite3_exec(sqlite3_log_in, sql_cmd, NULL, NULL, &errmsg);
    if (SQLITE_OK != sqlite_Errnum)
    {
        printf("sqlite3_exec : errnum:[%d] errstr:[%s]\n", sqlite_Errnum, errmsg);
        sqlite3_close(sqlite3_log_in);
        sqlite3_free(errmsg);
        exit(-1);
    }

    // 释放报错信息
    sqlite3_free(errmsg);

    return sqlite3_log_in;
}


void sqlite3_creat_table(sqlite3 *sql, char *User)
{
    char sql_cmd[128];
    int sqlite_Errnum = 0;
    char *errmsg;

    sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s (id INT, User CHAR(64) PRIMARY KEY, Online CHAR(64))", User);
    // INTEGER PRIMARY KEY AUTOINCREMENT 从 1 开始自增
    // IF NOT EXISTS 当有表格时 不会报错
    sqlite_Errnum = sqlite3_exec(sql, sql_cmd, NULL, NULL, &errmsg);
    if (SQLITE_OK != sqlite_Errnum)
    {
        printf("sqlite3_exec : errnum:[%d] errstr:[%s]\n", sqlite_Errnum, errmsg);
        sqlite3_close(sql);
        sqlite3_free(errmsg);
        exit(-1);
    }

    // 释放报错信息
    sqlite3_free(errmsg);

}

// 插入
/*
参数:  
    sql 数据库句柄
    User 用户名
    password 密码
返回值:
    无
*/
void Insert_INTO_log_in(sqlite3 *sql, char *User, char *password, char *table)
{

    // 数据库错误码
    int sql_err_num;
    // 数据库错误信息
    char *sql_err_str;

    // 数据库命令
    char sql_cmd[1024];

    // 命令组装
    snprintf(sql_cmd, sizeof(sql_cmd), "INSERT INTO %s (User, password) VALUES ('%s' , '%s')",
             Tab_name, User, password);

    // 执行命令
    sql_err_num = sqlite3_exec(sql, sql_cmd, NULL, NULL, &sql_err_str);
    if (SQLITE_OK != sql_err_num)
    {
        printf("插入错误 : errnum:[%d] errstr:[%s]\n", sql_err_num, sql_err_str);
        sqlite3_free(sql_err_str);
        return;
    }

    printf("插入成功\n");

    return;
}

void Insert_INTO_Table(sqlite3 *sql,int id, char *User, char *Online, char *table)
{

    // 数据库错误码
    int sql_err_num;
    // 数据库错误信息
    char *sql_err_str;

    // 数据库命令
    char sql_cmd[1024];

    // 命令组装
    snprintf(sql_cmd, sizeof(sql_cmd), "INSERT INTO %s VALUES ('%d' , '%s' , '%s')",
             table,id, User, Online);

    // 执行命令
    sql_err_num = sqlite3_exec(sql, sql_cmd, NULL, NULL, &sql_err_str);
    if (SQLITE_OK != sql_err_num)
    {
        printf("插入错误 : errnum:[%d] errstr:[%s]\n", sql_err_num, sql_err_str);
        sqlite3_free(sql_err_str);
        return;
    }

    printf("插入成功\n");

    return;
}

// 删除
void sqlite3_Delete_FROM_log_in(sqlite3 *sql, int id ,char *table)
{
    // 数据库错误码
    int sql_err_num;
    // 数据库错误信息
    char *sql_err_str;
    // 数据库命令
    char sql_cmd[1024];

    // 组装命令 DELETE FROM log_in WHERE id = 1001;
    snprintf(sql_cmd, sizeof(sql_cmd), "DELETE FROM %s WHERE id = %d",
             table, id);

    // 执行命令
    sql_err_num = sqlite3_exec(sql, sql_cmd, NULL, NULL, &sql_err_str);
    if (SQLITE_OK != sql_err_num)
    {
        printf("插入错误 : errnum:[%d] errstr:[%s]\n", sql_err_num, sql_err_str);
        sqlite3_free(sql_err_str);
        return;
    }

    printf("删除成功\n");

    return;
}

// 查找
/*
参数:  
    sql 数据库句柄
    User 用户名
返回值:
    当前用户名的密码
*/
char *Select_FROM_log_in_passwd(sqlite3 *sql, char *User)
{
    // 用户id
    char *password = (char *)malloc(sizeof(char) * 64);
    memset(password , 0 , sizeof(char) * 64);
    // 数据库错误码
    int sql_err_num;
    // 数据库错误信息
    char *sql_err_str;
    // 数据库命令
    char sql_cmd[1024];

    // 组装命令 SELECT * FROM 表名 WHERE 条件;
    snprintf(sql_cmd, sizeof(sql_cmd), "SELECT password FROM %s WHERE User = '%s'",
             Tab_name, User);

    // 执行命令
    sql_err_num = sqlite3_exec(sql, sql_cmd, find_User_Password , (void *)password, &sql_err_str);
    if (SQLITE_OK != sql_err_num)
    {
        printf("查询错误 : errnum:[%d] errstr:[%s]\n", sql_err_num, sql_err_str);
        sqlite3_free(sql_err_str);

        printf("未找到\n");

        return NULL;
    }

    if (0 == password[0])
    {
        printf("未找到\n");
        free(password);
        return NULL;
    }
    else
    {
        printf("找到了\n");
        return password;
    }
}


// 查找
/*
参数:  
    sql 数据库句柄
    User 用户名
返回值:
    当前用户名的id
*/
int sqlite3_Select_User_log_in(sqlite3 *sql, char *User , char *table)
{
    // 用户id
    int id = 0;

    // 数据库错误码
    int sql_err_num;
    // 数据库错误信息
    char *sql_err_str;
    // 数据库命令
    char sql_cmd[1024];

    // 组装命令 SELECT * FROM 表名 WHERE 条件;
    snprintf(sql_cmd, sizeof(sql_cmd), "SELECT id FROM %s WHERE User = '%s'",
             table, User);

    // 执行命令
    sql_err_num = sqlite3_exec(sql, sql_cmd, callback_name, (void *)&id, &sql_err_str);
    if (SQLITE_OK != sql_err_num)
    {
        printf("查询错误 : errnum:[%d] errstr:[%s]\n", sql_err_num, sql_err_str);
        sqlite3_free(sql_err_str);
        // return EOF;
    }

    printf("找到了\n");
    return id;

    printf("没找到\n");
}


//遍历数据库
int sqlite3_printf_log_in(sqlite3 *sql,int Sock_fd , char *table)
{
    // 数据库错误码
    int sql_err_num;
    // 数据库错误信息
    char *sql_err_str;
    // 数据库命令
    char sql_cmd[1024];

    // 组装命令 SELECT * FROM 表名 WHERE 条件;
    snprintf(sql_cmd, sizeof(sql_cmd), "SELECT * FROM  %s ",table);

    sql_err_num = sqlite3_exec(sql, sql_cmd, callback_prient, (void *)&Sock_fd, &sql_err_str);
    if (sql_err_num != SQLITE_OK ) {
        
        printf("遍历错误 : errnum:[%d] errstr:[%s]\n", sql_err_num, sql_err_str);
        sqlite3_free(sql_err_str);
    }

    return 0;
}