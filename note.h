#ifndef NOTE_H
#define NOTE_H

#include "sqlutils.h"
#include <QString>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QtSql>

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
        void setTitle(const QString & title);
        void setContent(const QString & content);
        void setId(const int id);
        void setSharedKey(const QString & sharedKey);
        void setCreatedAt(const QString &createdAt);
        void setUpdatedAt(const QString & updatedAt);
        void setSyncedAt(const QString &syncedAt);
        void setToSync(const bool toSync);
        static QList<Note> readFromFile();
        static void writeToFile(const QList<Note> & notes);
        static void createNotesTableIfNotExists();
        static QString getJsonNotesToSync();
        static QList<Note> loadFromDb();
        void addToDb();
        void editInDb();
        static void deleteInDb();
        static void deleteInDb(const QString & sharedKey);
        static void setToSyncOffInDb();
        static bool exists(const QString & sharedKey);
        void setDeleteInDb();
        int lastInsertId();

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
};

#endif // NOTE_H
