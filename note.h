#ifndef NOTE_H
#define NOTE_H

#include "sqlutils.h"
#include <QtSql>
#include <QString>
#include <QFile>
#include <QList>
#include <QTextStream>
class NoteListWidgetItem;
class NoteDialog;
class NotePanel;

class Note
{
    public:
        Note();
        Note(const QString &title, const QString &content, const QString &sharedKey = "");
        QString title() const;
        QString content() const;
        int id() const;
        QString sharedKey() const;
        QDateTime createdAt() const;
        QDateTime updatedAt() const;
        QDateTime syncedAt() const;
        bool toSync() const;
        NoteListWidgetItem* item();
        NoteDialog* noteDialog();
        void setTitle(const QString & title);
        void setContent(const QString & content);
        void setId(const int id);
        void setSharedKey(const QString & sharedKey);
        void setCreatedAt(const QString &createdAt);
        void setUpdatedAt(const QString & updatedAt);
        void setSyncedAt(const QString &syncedAt);
        void setToSync(const bool toSync);
        void setItem(NoteListWidgetItem* item);
        void setNoteDialog(NoteDialog *noteDialog);
        static QList<Note> readFromFile();
        static void writeToFile(const QList<Note> & notes);
        static QString getJsonNotesToSync();
        static QList<Note *> loadFromDb();
        static void deleteInDb();
        static void deleteInDb(const QString & sharedKey);
        static bool exists(const QString & sharedKey);
        void setSyncedNow();
        void setDeleteInDb();
        int lastInsertId();
        void addTag(const QString& name);
        void save(bool now = true);
        bool isInPanel();

    protected:
        void addToDb(bool createdNow);
        void editInDb(bool updatedNow);

    private:
        int m_id;
        QString m_title;
        QString m_content;
        QString m_sharedKey;
        bool m_toSync;
        bool m_toDelete;
        QDateTime m_createdAt;
        QDateTime m_updatedAt;
        QDateTime m_syncedAt;
        NoteListWidgetItem *m_item;
        NoteDialog *m_noteDialog;
};

#endif // NOTE_H
