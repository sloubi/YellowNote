#include "notelistwidgetitem.h"
#include "note.h"
#include "notelabel.h"

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

void NoteListWidgetItem::update()
{
    QListWidget *list = listWidget();
    NoteLabel *label = dynamic_cast<NoteLabel*>(list->itemWidget(this));
    label->update();
    setSizeHint(label->minimumSizeHint());
}
