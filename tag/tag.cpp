#include "tag.h"
#include "sqlutils.h"

Tag::Tag()
{

}

Tag::Tag(int id, const QString& name)
    : m_id(id), m_name(name)
{

}

int Tag::id() const
{
    return m_id;
}

QString Tag::name() const
{
    return m_name;
}

void Tag::setId(int id)
{
    m_id = id;
}

void Tag::setName(const QString & name)
{
    m_name = name;
}

QList<Tag*> Tag::findByNoteId(int note_id)
{
    QList<Tag*> tags;

    SqlQuery query;
    query.prepare("SELECT t.id, t.name "
                   "FROM tags t "
                   "INNER JOIN notes_tags nt ON t.id = nt.tag_id "
                   "WHERE nt.note_id = :note_id");
    query.bindValue(":note_id", note_id);
    query.exec();

    while (query.next())
    {
        Tag *tag = new Tag(query.value(0).toInt(), query.value(1).toString());
        tags.append(tag);
    }

    return tags;
}
