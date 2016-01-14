#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QtSql>

class Note
{
    public:
        Note();
        Note(const QString &title, const QString &content);
        QString title() const;
        QString content() const;
        int id() const;
        void setTitle(const QString & title);
        void setContent(const QString & content);
        void setId(const int id);
        static QList<Note> readFromFile();
        static void writeToFile(const QList<Note> & notes);
        static void createNotesTableIfNotExists();
        static QList<Note> loadFromDb();
        void addToDb();
        void editInDb();
        void deleteInDb();
        int lastInsertId();

    private:
        int m_id;
        QString m_title;
        QString m_content;
};

#endif // NOTE_H
