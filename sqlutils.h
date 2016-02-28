#ifndef SQLUTILS_H
#define SQLUTILS_H

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

class SqlUtils
{
public:
    static QString date(const QDateTime & dateTime);
    static QDateTime date(const QString & string);
    static QString getNow();
    static int lastInsertId();
    static QString sqlLiteVersion();
    static QString dbPath();
};

#endif // SQLUTILS_H
