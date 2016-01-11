#include "note.h"

Note::Note()
{

}

Note::Note(const QString & title, const QString & content) : m_title(title), m_content(content)
{

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
    query.exec("CREATE TABLE if not exists notes ('id' INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL, 'title'  NOT NULL, 'content'  NOT NULL)");
}

QList<Note> Note::loadFromDb()
{
    QSqlDatabase db = QSqlDatabase::database();
    QList<Note> notes;

    createNotesTableIfNotExists();

    QSqlQuery query(db);
    if (query.exec("SELECT * FROM notes"))
    {
        while (query.next())
        {
            Note note(query.value(1).toString(), query.value(2).toString());
            note.setId(query.value(0).toInt());
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
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("INSERT INTO notes('title', 'content') VALUES (:title, :content)");
    q.bindValue(":title", m_title);
    q.bindValue(":content", m_content);
    q.exec();
}
