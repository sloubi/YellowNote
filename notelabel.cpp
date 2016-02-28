#include "notelabel.h"

NoteLabel::NoteLabel(Note *note) : QLabel()
{
    m_note = note;

    setWordWrap(true);
    setMargin(10);

    QFontMetrics fm(this->font());
    // Hauteur de 3 lignes + les marges
    setFixedHeight(fm.height() * 3 + 20);

    setAlignment(Qt::AlignTop);

    update();
}

void NoteLabel::setItem(NoteListWidgetItem *item)
{
    m_item = item;
}

void NoteLabel::update()
{
    QString title = m_note->title().left(40);
    if (title != m_note->title())
        title += "...";

    QString date = m_note->updatedAt().toString("d MMM yyyy");

    setText("<font color='#1F1F1F' size='3'>" + title + "</font><br>"
            "<font color='#808080'>" + date + "</font> - "
            "<font color='#545454'>" + m_note->content() + "</font>");
}
