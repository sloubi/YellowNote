#ifndef NOTELABEL_H
#define NOTELABEL_H

#include <QLabel>
#include <QString>
#include "note.h"

class NoteLabel : public QLabel
{
    public:
        NoteLabel(Note *note);
        void update();
        void setItem(NoteListWidgetItem *item);

    private:
        Note *m_note;
        NoteListWidgetItem *m_item;
};

#endif // NOTELABEL_H
