#ifndef TAGEDIT_H
#define TAGEDIT_H

#include <QHBoxLayout>
#include <QLineEdit>
#include <QKeyEvent>
#include <QFocusEvent>
#include "note.h"

class TagEdit : public QLineEdit
{
public:
    TagEdit(QHBoxLayout *container, Note *note, QWidget *parent = 0);
    void setNote(Note *note);
protected:
    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void add();

    QHBoxLayout *m_container;
    Note *m_note;
};

#endif // TAGEDIT_H
