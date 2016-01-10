#include "notelabel.h"

NoteLabel::NoteLabel(const QString & title, const QString & text) : QLabel(text)
{
    m_title = title;

    setStyleSheet("border: 1px solid blue");
    setText(m_title + "<br>" + text);
}

void NoteLabel::setTitle(const QString & text)
{
    m_title = text;
}

void NoteLabel::dbClicked()
{
    QMessageBox::information(this, "Titre de la fenêtre", "coucou");
}
