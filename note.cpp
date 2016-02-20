#include <QDebug>
#include "note.h"
#include "notelistwidgetitem.h"

Note::Note()
{
}

Note::Note(const QString & title, const QString & content, const QString & sharedKey)
    : m_title(title), m_content(content), m_sharedKey(sharedKey)
{
    // Génération d'un titre si il est vide
    // Seulement pour les nouvelles notes du coup
    // Dans la modification d'une note, on utilise le setter
    if (title == "")
    {
        m_title = m_content.left(40);
        if (m_content.size() > 40)
            m_title += "...";
    }

    if (sharedKey == "")
    {
        m_sharedKey = QUuid::createUuid().toString();
    }

    m_toSync = true;
    m_item = 0;
}

QString Note::title() const
{
    return m_title;
}

QString Note::content() const
{
    return m_content;
}

int Note::id() const
{
    return m_id;
}

QString Note::sharedKey() const
{
    return m_sharedKey;
}

QDateTime Note::createdAt() const
{
    return m_createdAt;
}

QDateTime Note::updatedAt() const
{
    return m_updatedAt;
}

QDateTime Note::syncedAt() const
{
    return m_syncedAt;
}

bool Note::toSync() const
{
    return m_toSync;
}

NoteListWidgetItem* Note::item()
{
    return m_item;
}

void Note::setTitle(const QString & title)
{
    m_title = title;
}

void Note::setContent(const QString & content)
{
    m_content = content;
}

void Note::setId(const int id)
{
    m_id = id;
}

void Note::setSharedKey(const QString & sharedKey)
{
    m_sharedKey = sharedKey;
}

void Note::setCreatedAt(const QString & createdAt)
{
    m_createdAt = SqlUtils::date(createdAt);
}

void Note::setUpdatedAt(const QString & updatedAt)
{
    m_updatedAt = SqlUtils::date(updatedAt);
}

void Note::setSyncedAt(const QString & syncedAt)
{
    m_syncedAt = SqlUtils::date(syncedAt);
}

void Note::setToSync(const bool toSync)
{
    m_toSync = toSync;
}

void Note::setItem(NoteListWidgetItem *item)
{
    m_item = item;
}

QList<Note> Note::readFromFile()
{
    QFile file("C:/test.txt");
    QList<Note> notes;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QStringList line = in.readLine().split(";");
            Note note(line.first(), line.last());
            notes.append(note);
        }
    }

    return notes;
}

void Note::writeToFile(const QList<Note> & notes)
{
    QFile file("C:/test.txt");
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        for (int i = 0; i < notes.size(); ++i)
        {
            stream << notes.at(i).title() << ";" << notes.at(i).content() << endl;
        }
    }
}

void Note::createNotesTableIfNotExists()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.exec("CREATE TABLE 'notes' ("
                   "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                   "'shared_key' VARCHAR NOT NULL,"
                   "'title' VARCHAR NOT NULL,"
                   "'content' TEXT NOT NULL,"
                   "'to_sync' BOOL NOT NULL DEFAULT (0),"
                   "'to_delete' BOOL NOT NULL DEFAULT (0),"
                   "'created_at' DATETIME NOT NULL,"
                   "'updated_at' DATETIME NOT NULL,"
                   "'synced_at' DATETIME NULL DEFAULT (NULL)"
               ")");
}

QList<Note*> Note::loadFromDb()
{
    QSqlDatabase db = QSqlDatabase::database();
    QList<Note*> notes;

    createNotesTableIfNotExists();

    QSqlQuery query(db);

    if (query.exec("SELECT id, shared_key, title, content, to_sync, to_delete, "
                   "datetime(created_at) AS created_at, "
                   "datetime(updated_at) AS updated_at, "
                   "datetime(synced_at) AS synced_at "
                   "FROM notes "
                   "WHERE to_delete = 0"))
    {
        while (query.next())
        {
            Note *note = new Note(query.value(2).toString(), query.value(3).toString(), query.value(1).toString());
            note->setId(query.value(0).toInt());
            note->setCreatedAt(query.value(6).toString());
            note->setUpdatedAt(query.value(7).toString());
            note->setSyncedAt(query.value(8).toString());
            note->setToSync(query.value(4).toBool());
            notes.append(note);
        }
    }
    else
    {
        qDebug() << "sql error : " << query.lastError();
    }

    return notes;
}

void Note::addToDb()
{
    // Si la note est à synchroniser
    if (m_toSync)
    {
        // On met les dates de création/modification à maintenant
        m_createdAt = QDateTime::currentDateTime();
        m_updatedAt = QDateTime::currentDateTime();
    }

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("INSERT INTO notes('shared_key', 'title', 'content', 'to_sync', 'created_at', 'updated_at') "
              "VALUES (:shared_key, :title, :content, :to_sync, :created_at, :updated_at)");
    q.bindValue(":shared_key", m_sharedKey);
    q.bindValue(":title", m_title);
    q.bindValue(":content", m_content);
    q.bindValue(":to_sync", m_toSync);
    q.bindValue(":created_at", SqlUtils::date(m_createdAt));
    q.bindValue(":updated_at", SqlUtils::date(m_updatedAt));

    if (!q.exec())
    {
        qDebug() << "sql error : " << q.lastError();
    }

    m_id = SqlUtils::lastInsertId();
}



void Note::editInDb(bool setUpdatedAt)
{
    if (setUpdatedAt)
        m_updatedAt = QDateTime::currentDateTime();

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("UPDATE notes SET title = :title, content = :content, to_sync = 1, updated_at = :updated_at WHERE id = :id");
    q.bindValue(":title", m_title);
    q.bindValue(":content", m_content);
    q.bindValue(":updated_at", m_updatedAt);
    q.bindValue(":id", m_id);
    q.exec();
}

void Note::deleteInDb()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("DELETE FROM notes WHERE to_delete = 1");
    q.exec();
}

void Note::deleteInDb(const QString & sharedKey)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("DELETE FROM notes WHERE shared_key = :shared_key");
    q.bindValue(":shared_key", sharedKey);
    q.exec();
}

void Note::setDeleteInDb()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("UPDATE notes SET to_delete = 1, to_sync = 1 WHERE id = :id");
    q.bindValue(":id", m_id);
    q.exec();
}

void Note::setToSyncOffInDb()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("UPDATE notes SET to_sync = 0, synced_at = datetime('now', 'localtime') WHERE to_sync = 1");
    q.exec();
}

bool Note::exists(const QString & sharedKey)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM notes WHERE shared_key = :shared_key");
    q.bindValue(":shared_key", sharedKey);
    q.exec();
    q.next();
    return q.value(0).toBool();
}

QString Note::getJsonNotesToSync()
{
    QSqlDatabase db = QSqlDatabase::database();
    QJsonArray jsonNotes;

    QSqlQuery query(db);
    query.exec("SELECT shared_key, title, content, to_delete, "
                   "datetime(created_at) AS created_at, "
                   "datetime(updated_at) AS updated_at "
               "FROM notes "
               "WHERE to_sync = 1");
    while (query.next())
    {
        QJsonObject jsonNote;
        jsonNote["shared_key"] = query.value(0).toString();
        jsonNote["title"]      = query.value(1).toString();
        jsonNote["content"]    = query.value(2).toString();
        jsonNote["to_delete"]  = query.value(3).toString();
        jsonNote["created_at"] = query.value(4).toString();
        jsonNote["updated_at"] = query.value(5).toString();

        jsonNotes.append(jsonNote);
    }

    return QJsonDocument(jsonNotes).toJson(QJsonDocument::Compact);
}
