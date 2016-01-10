#include "database.h"

Database Database::m_instance=Database();

Database::Database()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("notes.db");
    m_db.open();
}

Database::~Database()
{
    m_db.close();
}

Database& Database::Instance()
{
    return m_db;
}

