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

    private:
        Note *m_note;
};

#endif // NOTELABEL_H
