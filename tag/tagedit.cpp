#include <QDebug>
#include <QCompleter>
#include "tagedit.h"
#include "tagwidget.h"

TagEdit::TagEdit(QHBoxLayout *container, Note *note, QWidget *parent) : QLineEdit(parent)
{
    QStringList wordList;
    wordList << "alpha" << "omega" << "omicron" << "zeta";

    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    setCompleter(completer);
    setStyleSheet("background-color: transparent; border: 0;");
    setPlaceholderText("Ajouter un tag");

    m_container = container;
    m_note = note;
}

void TagEdit::keyPressEvent(QKeyEvent *event)
{
    QLineEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        add();
    }
}

void TagEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    add();
}

void TagEdit::setNote(Note *note)
{
    m_note = note;
}

void TagEdit::add()
{
    if (text().isEmpty())
        return;

    TagWidget *t = new TagWidget(text());
    m_container->addWidget(t);

    if (m_note)
        m_note->addTag(text());

    clear();
}
