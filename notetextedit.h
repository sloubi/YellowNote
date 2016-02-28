#ifndef NOTETEXTEDIT_H
#define NOTETEXTEDIT_H

#include <QTextEdit>
#include <QEvent>

class NoteTextEdit : public QTextEdit
{
public:
    NoteTextEdit();

protected:
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
};

#endif // NOTETEXTEDIT_H
