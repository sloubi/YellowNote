#ifndef TAGCONTAINER_H
#define TAGCONTAINER_H

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include "tagedit.h"
#include "note.h"

class TagContainer : public QWidget
{
public:
    TagContainer(Note *note, QWidget *parent = 0);
    void add();
    void setNote(Note *note);
    void update();

protected:
    void clearContainer();

    QHBoxLayout *m_container;
    TagEdit *m_tagEdit;
    Note *m_note;
};

#endif // TAGCONTAINER_H
