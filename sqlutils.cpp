#include "sqlutils.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>

QString SqlUtils::date(const QDateTime & dateTime)
{
    return dateTime.toString("yyyy-MM-dd hh:mm:ss");
}

QDateTime SqlUtils::date(const QString & string)
{
    return QDateTime::fromString(string, Qt::ISODate);
}

QString SqlUtils::getNow()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

int SqlUtils::lastInsertId()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.exec("SELECT last_insert_rowid()");
    q.next();
    return q.value(0).toInt();
}

QString SqlUtils::sqlLiteVersion()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.exec("SELECT sqlite_version()");
    q.next();
    return q.value(0).toString();
}

QString SqlUtils::dbPath()
{
    QSqlDatabase db = QSqlDatabase::database();
    return QCoreApplication::applicationDirPath() + "/" + db.databaseName();
}

bool SqlQuery::exec()
{
    bool result = QSqlQuery::exec();
    if (!result)
    {
        qDebug() << lastQuery() << "ERROR: " << lastError();
    }
    return result;
}

bool SqlQuery::exec(const QString & query)
{
    bool result = QSqlQuery::exec(query);
    if (!result)
    {
        qDebug() << lastQuery() << "ERROR: " << lastError();
    }
    return result;
}
