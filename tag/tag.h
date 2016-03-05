#ifndef TAG_H
#define TAG_H

#include <QList>
#include <QString>

class Tag
{
public:
    Tag();
    Tag(int id, const QString& name);
    int id() const;
    QString name() const;
    void setId(int id);
    void setName(const QString & name);
    static QList<Tag*> findByNoteId(int note_id);
protected:
    int m_id;
    QString m_name;
};

#endif // TAG_H
