#include "notelistwidgetitem.h"

NoteListWidgetItem::NoteListWidgetItem(QListWidget *view) : QListWidgetItem(view)
{

}

Note* NoteListWidgetItem::note() const
{
    return m_note;
}

void NoteListWidgetItem::setNote(Note *note)
{
    m_note = note;
}
