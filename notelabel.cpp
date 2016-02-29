#include "notelabel.h"
#include <qDebug>

NoteLabel::NoteLabel(Note *note) : QLabel()
{
    m_note = note;

    setWordWrap(true);
    setMargin(10);

    // Corrige un bug étrange
    // La font est 1 pt plus grosse quand on ajoute une note
    // (mais pas quand la note est ajoutée depuis l'initialisation de l'appli)
    QFont f("MS Shell Dlg 2", 8);
    setFont(f);

    QFontMetrics fm(font());
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
