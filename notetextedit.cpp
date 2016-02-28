#include "notetextedit.h"
#include <QDebug>


NoteTextEdit::NoteTextEdit() : QTextEdit()
{

}

void NoteTextEdit::focusOutEvent(QFocusEvent *event)
{
    qDebug() << "perte focus 2";
    QTextEdit::focusOutEvent(event);
}

