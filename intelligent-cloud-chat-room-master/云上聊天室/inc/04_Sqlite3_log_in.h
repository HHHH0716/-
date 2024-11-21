#ifndef __SQLLITE_3_LOG_IN_H__
#define __SQLLITE_3_LOG_IN_H__


#define Tab_name "log_in"

#include "04_Sqlite3_log_in.h"
#include "sys_head.h"

// 回调函数
int find_User_Password(void *Password, int argc, char **argv, char **azColName);

// 回调函数
int find_User_id(void *data, int argc, char **argv, char **azColName);
int callback_name(void *data, int argc, char **argv, char **azColName);
int callback_prient(void *data, int argc, char **argv, char **azColName);

sqlite3 *sqlite3_init_log_in(void);

void sqlite3_creat_table(sqlite3 *sql, char *User);


// 插入
void Insert_INTO_log_in(sqlite3 *sql, char *User, char *password, char *table);
void Insert_INTO_Table(sqlite3 *sql,int id, char *User, char *Online, char *table);

// 删除
void sqlite3_Delete_FROM_log_in(sqlite3 *sql, int id ,char *table);

// 查找
int sqlite3_Select_User_log_in(sqlite3 *sql, char *User , char *table);
char *Select_FROM_log_in_passwd(sqlite3 *sql, char *User);

// 修改

void Updata_SET_log_in(sqlite3 *sql);
//遍历
int sqlite3_printf_log_in(sqlite3 *sql,int Sock_fd , char *table);

#endif