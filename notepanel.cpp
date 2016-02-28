#include "notepanel.h"
#include "note.h"
#include <QDebug>

NotePanel::NotePanel() : NoteDialog()
{
    // La perte du focus au changement de fenêtre ne peut pas fonctionner
    // dans ce cas. On sauvegarde donc à la perte du focus du champ title ou content
    connect(m_noteEdit, SIGNAL(lostFocus()), this, SLOT(save()));

    // On écrase le slot de suppression
    m_actionDelete->disconnect();
    connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(deleteMe()));
}

// Sucharge de cette fonction car on ne doit pas fermer le widget
// après une suppression
void NotePanel::deleteMe()
{
    // signal de suppression
    if (m_note)
    {
        emit deletionRequested(m_note);
    }

    m_noteEdit->clear();
    m_noteEdit->setNoChange();

    m_note = 0;
}

// On ne doit pas attacher le noteDialog à la note
void NotePanel::attachToNote()
{
    return;
}

bool NotePanel::save()
{
    bool saved = NoteDialog::save();

    // Si on a sauvegardé le panneau de la note
    // et que la note est ouverte dans un dialog, on met à jour le dialog
    if (saved && m_note->noteDialog())
    {
        m_note->noteDialog()->update();
    }

    return saved;
}
