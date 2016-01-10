#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>

class Database
{
public:
    static Database& Instance();
private:
    Database& operator= (const Database&){}
    Database (const Database&){}

    static Database m_instance;
    QSqlDatabase m_db;
    Database();
    ~Database();
};

#endif // DATABASE_H
