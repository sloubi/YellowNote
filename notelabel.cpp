#include "notelabel.h"

NoteLabel::NoteLabel(Note *note)
{
    m_note = note;
    update();
}

void NoteLabel::update()
{
    QString title = m_note->title().left(40);
    if (title != m_note->title())
        title += "...";

    QString content = m_note->content().left(40);
    if (content != m_note->content())
        content += "...";

    setStyleSheet("padding: 10px;");
    setText("<font color='#1F1F1F' size='3'>" + title + "</font><br>"
            "<font color='#808080'>" + m_note->updatedAt().toString("d MMM yyyy") + "</font> - "
            "<font color='#545454'>" + content + "</font>");
}
