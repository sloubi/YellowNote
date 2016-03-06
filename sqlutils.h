#ifndef SQLUTILS_H
#define SQLUTILS_H

#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>

class SqlUtils
{
public:
    static void openDb();
    static QString date(const QDateTime & dateTime);
    static QDateTime date(const QString & string);
    static QString getNow();
    static int lastInsertId();
    static QString sqlLiteVersion();
    static QString dbPath();
protected:
    static void createTables();
};

class SqlQuery : public QSqlQuery
{
public:
    bool exec();
    bool exec(const QString & query);
};

#endif // SQLUTILS_H
