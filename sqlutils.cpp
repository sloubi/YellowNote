#include "sqlutils.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFile>

void SqlUtils::openDb()
{
    QFile dbFile("notes.db");
    bool emptyDb = !dbFile.exists();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("notes.db");
    db.open();

    if (emptyDb)
        createTables();
}

void SqlUtils::createTables()
{
    SqlQuery query;
    query.exec("CREATE TABLE notes ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                "shared_key VARCHAR NOT NULL,"
                "title VARCHAR NOT NULL,"
                "content TEXT NOT NULL,"
                "to_sync BOOL NOT NULL DEFAULT (0),"
                "to_delete BOOL NOT NULL DEFAULT (0),"
                "created_at DATETIME NOT NULL,"
                "updated_at DATETIME NOT NULL,"
                "synced_at DATETIME NULL DEFAULT (NULL))");

    query.exec("CREATE TABLE notes_tags ("
                "note_id INTEGER NOT NULL,"
                "tag_id INTEGER NOT NULL,"
                "PRIMARY KEY (note_id, tag_id))");

    query.exec("CREATE TABLE tags ("
                "id INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL,"
                "name VARCHAR)");
}

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
    SqlQuery q;
    q.exec("SELECT last_insert_rowid()");
    q.next();
    return q.value(0).toInt();
}

QString SqlUtils::sqlLiteVersion()
{
    SqlQuery q;
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
