#ifndef NOTELISTWIDGETITEM_H
#define NOTELISTWIDGETITEM_H

#include <QListWidgetItem>
#include "note.h"

class NoteListWidgetItem : public QListWidgetItem
{
public:
    NoteListWidgetItem(QListWidget *view = 0);
    Note note() const;
    void setNote(const Note &note);

private:
    Note m_note;
};

#endif // NOTELISTWIDGETITEM_H
