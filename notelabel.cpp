#include "notelabel.h"

NoteLabel::NoteLabel(const QString & title, const QString & content, const QDateTime &updatedAt)
{
    m_title = title;
    m_content = content;
    m_updatedAt = updatedAt;

    print();
}

NoteLabel::NoteLabel(const Note & note)
{
    m_title = note.title();
    m_content = note.content();
    m_updatedAt = note.updatedAt();

    print();
}

void NoteLabel::print()
{
    QString title = m_title.left(40);
    if (title != m_title)
        title += "...";

    QString content = m_content.left(40);
    if (content != m_content)
        content += "...";

    setStyleSheet("padding: 10px;");
    setText("<font color='#1F1F1F' size='3'>" + title + "</font><br>"
            "<font color='#808080'>" + m_updatedAt.toString("d MMM yyyy") + "</font> - "
            "<font color='#545454'>" + content + "</font>");
}
