#ifndef __LOG_IN_H__
#define __LOG_IN_H__

#include <sys_head.h>

// 登陆
/*
功能 通过传入 的用户名和密码 去数据库中进行比对如果有
成功 
    返回 true
    失败 false
*/

bool log_in(sqlite3 * db , char  * name , char * password);

// 注册
/*
功能 通过传入 的用户名和密码 去数据库中进行插入操作 (不能有重名用户)
成功 
    返回 true
    失败 false
*/

bool Register(sqlite3 * db , char  * name , char * password);






#endif
