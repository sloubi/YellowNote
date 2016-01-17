#include "notelabel.h"

NoteLabel::NoteLabel(const QString & title, const QString & content)
{
    m_title = title;
    m_content = content;

    print();
}

NoteLabel::NoteLabel(const Note & note)
{
    m_title = note.title();
    m_content = note.content();

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
    setText(title + "<br><font color='#5C5C5C'>" + content + "</font>");
}
