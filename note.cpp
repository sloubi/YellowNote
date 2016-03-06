#include <QDebug>
#include "note.h"
#include "notelistwidgetitem.h"
#include "notedialog.h"
#include "notepanel.h"
#include "tag/tagwidget.h"

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
        QString firstLine = m_content.split(QRegExp("[\r\n]"), QString::SkipEmptyParts).at(0);
        m_title = firstLine.left(40);
        if (firstLine.size() > 40)
            m_title += "...";
    }

    if (sharedKey == "")
    {
        m_sharedKey = QUuid::createUuid().toString();
    }

    m_toSync = true;
    m_item = 0;
    m_noteDialog = 0;
    m_notePanel = 0;
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

NoteDialog* Note::noteDialog()
{
    return m_noteDialog;
}

NotePanel* Note::notePanel()
{
    return m_notePanel;
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

void Note::setNoteDialog(NoteDialog *noteDialog)
{
    m_noteDialog = noteDialog;
}

void Note::setNotePanel(NotePanel *notePanel)
{
    m_notePanel = notePanel;
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

QList<Note*> Note::loadFromDb()
{
    QList<Note*> notes;

    SqlQuery query;
    query.exec("SELECT id, shared_key, title, content, to_sync, to_delete, "
                   "datetime(created_at) AS created_at, "
                   "datetime(updated_at) AS updated_at, "
                   "datetime(synced_at) AS synced_at "
                   "FROM notes "
                   "WHERE to_delete = 0");

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
    return notes;
}

void Note::addToDb(bool createdNow)
{
    // Si la note vient d'être créée
    if (createdNow)
    {
        // On met les dates de création/modification à maintenant
        m_createdAt = QDateTime::currentDateTime();
        m_updatedAt = QDateTime::currentDateTime();
    }

    SqlQuery q;
    q.prepare("INSERT INTO notes('shared_key', 'title', 'content', 'to_sync', 'created_at', 'updated_at') "
              "VALUES (:shared_key, :title, :content, :to_sync, :created_at, :updated_at)");
    q.bindValue(":shared_key", m_sharedKey);
    q.bindValue(":title", m_title);
    q.bindValue(":content", m_content);
    q.bindValue(":to_sync", m_toSync);
    q.bindValue(":created_at", SqlUtils::date(m_createdAt));
    q.bindValue(":updated_at", SqlUtils::date(m_updatedAt));
    q.exec();

    m_id = SqlUtils::lastInsertId();
}

void Note::editInDb(bool updatedNow)
{
    if (updatedNow)
        m_updatedAt = QDateTime::currentDateTime();

    SqlQuery q;
    q.prepare("UPDATE notes SET "
              "title = :title, content = :content, to_sync = :to_sync, updated_at = :updated_at "
              "WHERE id = :id");
    q.bindValue(":title", m_title);
    q.bindValue(":content", m_content);
    q.bindValue(":to_sync", m_toSync);
    q.bindValue(":updated_at", SqlUtils::date(m_updatedAt));
    q.bindValue(":id", m_id);
    q.exec();
}

void Note::deleteInDb()
{
    SqlQuery q;
    q.prepare("DELETE FROM notes WHERE to_delete = 1");
    q.exec();
}

void Note::deleteInDb(const QString & sharedKey)
{
    SqlQuery q;
    q.prepare("DELETE FROM notes WHERE shared_key = :shared_key");
    q.bindValue(":shared_key", sharedKey);
    q.exec();
}

void Note::setDeleteInDb()
{
    m_toDelete = true;
    m_toSync = true;

    // Si la note n'a jamais été synchronisée,
    // on peut la supprimer totalement de la base
    if (m_syncedAt.isNull())
        deleteInDb(m_sharedKey);

    SqlQuery q;
    q.prepare("UPDATE notes SET to_delete = 1, to_sync = 1 WHERE id = :id");
    q.bindValue(":id", m_id);
    q.exec();
}

void Note::setSyncedNow()
{
    m_toSync = false;
    m_syncedAt = QDateTime::currentDateTime();

    SqlQuery q;
    q.prepare("UPDATE notes SET "
              "to_sync = 0, "
              "synced_at = :synced_at "
              "WHERE id = :id");
    q.bindValue(":synced_at", SqlUtils::date(m_syncedAt));
    q.bindValue(":id", m_id);
    q.exec();
}

bool Note::exists(const QString & sharedKey)
{
    SqlQuery q;
    q.prepare("SELECT COUNT(*) FROM notes WHERE shared_key = :shared_key");
    q.bindValue(":shared_key", sharedKey);
    q.exec();
    q.next();
    return q.value(0).toBool();
}

QString Note::getJsonNotesToSync()
{
    QJsonArray jsonNotes;

    SqlQuery query;
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

void Note::updateDisplay()
{
    m_item->update();

    if (m_noteDialog)
        m_noteDialog->update();

    if (m_notePanel)
        m_notePanel->update();
}

void Note::addTag(const QString& name)
{
    // Récupération de tous les tags existants
    QMap<QString, int> tags = TagWidget::all();
    int tagId = tags.value(name);

    // Création du tag si il n'existe pas
    if (!tagId)
    {
        tagId = TagWidget::insertInDb(name);
    }

    // Ajout du tag à la note
    SqlQuery q;
    q.prepare("INSERT INTO notes_tags('note_id', 'tag_id') "
              "VALUES (:note_id, :tag_id)");
    q.bindValue(":note_id", m_id);
    q.bindValue(":tag_id", tagId);
    !q.exec();
}
