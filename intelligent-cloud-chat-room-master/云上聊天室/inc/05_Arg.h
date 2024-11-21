#ifndef __05_AGR_H__
#define __05_AGR_H__




// 定义协议
typedef union agr
{
    char buf[1024];

    struct request // 请求下载
    {
        int bytes : 3; // 操作码  1
        char data[1000];
    } request;

    struct File_list // 文件名列表信息
    {
        int bytes : 3;      // 操作码  2
        int num : 3;        // 文件个数
        char data[1000];    // 
    } File_list;

    struct File_name // 文件名信息
    {
        int bytes : 3; // 操作码  3
        char data[1000];
    } File_name;

    struct File_data // 文件数据
    {
        int bytes : 3; // 操作码  4
        char data[1000];
    } File_data;

} agr;




#endif
