#define _CRT_SECURE_NO_WARNINGS
#include "mysql_db.h"
#include <mysql.h>
#include <iostream>
#include <string>

bool MysqlDB::Connect()
{
    conn = mysql_init(nullptr);
    if (!conn) {
        std::cout << "mysql_init 失败" << std::endl;
        return false;
    }
    if (!mysql_real_connect(conn, host, user, pwd, dbName, port, nullptr, 0))
    {
        std::cout << "数据库连接失败：" << mysql_error(conn) << std::endl;
        return false;
    }
    std::cout << "数据库连接成功！" << std::endl;
    return true;
}

void MysqlDB::Close()
{
    if (conn)
    {
        mysql_close(conn);
        conn = nullptr;
    }
}

bool MysqlDB::Register(std::string username, std::string password)
{
    if (CheckUserExist(username))
        return false;

    char sql[256] = { 0 };
    // 改成user_name
    sprintf(sql, "INSERT INTO user_account(user_name,password) VALUES('%s','%s')",
        username.c_str(), password.c_str());
    int ret = mysql_query(conn, sql);
    return ret == 0;
}

bool MysqlDB::LoginCheck(std::string username, std::string password)
{
    char sql[256] = { 0 };
    // 改成user_name
    sprintf(sql, "SELECT * FROM user_account WHERE user_name='%s' AND password='%s'",
        username.c_str(), password.c_str());

    if (mysql_query(conn, sql) != 0) return false;
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return false;
    int row = mysql_num_rows(res);
    mysql_free_result(res);
    return row > 0;
}

bool MysqlDB::CheckUserExist(std::string username)
{
    char sql[256] = { 0 };
    // 改成user_name
    sprintf(sql, "SELECT * FROM user_account WHERE user_name='%s'", username.c_str());
    if (mysql_query(conn, sql) != 0) return false;
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return false;
    int row = mysql_num_rows(res);
    mysql_free_result(res);
    return row > 0;
}
// 根据用户名获取用户ID
int MysqlDB::GetUserIdByName(std::string username)
{
    char sql[256] = { 0 };
    // 适配你建的表字段：user_name
    sprintf(sql, "SELECT id FROM user_account WHERE user_name='%s'", username.c_str());
    if (mysql_query(conn, sql) != 0) return -1;
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return -1;
    MYSQL_ROW row = mysql_fetch_row(res);
    int uid = -1;
    if (row) uid = atoi(row[0]);
    mysql_free_result(res);
    return uid;
}

// 保存单关通关耗时
bool MysqlDB::InsertLevelRecord(int userId, int level, float time)
{
    char sql[256] = { 0 };
    sprintf(sql, "INSERT INTO user_level_record(user_id,level_num,cost_time) VALUES(%d,%d,%.2f)",
        userId, level, time);
    return mysql_query(conn, sql) == 0;
}

// 查询用户某一关的最快通关时间
float MysqlDB::GetUserBestTime(int userId, int level)
{
    char sql[256] = { 0 };
    sprintf(sql, "SELECT MIN(cost_time) FROM user_level_record WHERE user_id=%d AND level_num=%d",
        userId, level);
    if (mysql_query(conn, sql) != 0) return 9999.0f;
    MYSQL_RES* res = mysql_store_result(conn);
    float best = 9999.0f;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row && row[0]) best = atof(row[0]);
    mysql_free_result(res);
    return best;
}