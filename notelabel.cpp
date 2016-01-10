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
    setStyleSheet("padding: 10px;");
    setText(m_title + "<br><font color='#5C5C5C'>" + m_content + "</font>");
}
