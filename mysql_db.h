#ifndef MYSQL_DB_H
#define MYSQL_DB_H

#include <string>

// 【修改这里】改成纯前置声明，不typedef，避免和mysql.h冲突
struct MYSQL;

class MysqlDB
{
private:
    MYSQL* conn;
    const char* host = "localhost";
    const char* user = "root";
    const char* pwd = "11111111"; // 改成你自己的数据库密码
    const char* dbName = "snake_game";
    unsigned int port = 3306;

public:
    bool Connect();
    void Close();
    bool Register(std::string username, std::string password);
    bool LoginCheck(std::string username, std::string password);
    bool CheckUserExist(std::string username);
    int GetUserIdByName(std::string username);
    bool InsertLevelRecord(int userId, int level, float time);
    float GetUserBestTime(int userId, int level);
};

#endif