#include "notepanel.h"
#include "note.h"
#include <QDebug>

// Initialisation du singleton à NULL
NotePanel *NotePanel::m_instance = NULL;

NotePanel::NotePanel() : NoteDialog()
{
    // La perte du focus au changement de fenêtre ne peut pas fonctionner
    // dans ce cas. On sauvegarde donc à la perte du focus du champ title ou content
    connect(m_noteEdit, SIGNAL(lostFocus()), this, SLOT(save()));

    // On écrase le slot de suppression
    m_actionDelete->disconnect();
    connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(deleteMe()));
}

NotePanel* NotePanel::getInstance()
{
    if (m_instance == NULL)
    {
        m_instance =  new NotePanel;
    }

    return m_instance;
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

void NotePanel::handleNoteChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (current)
    {
        // Mise à jour du panneau avec la note sélectionné
        NoteListWidgetItem *currentNoteItem = static_cast<NoteListWidgetItem*>(current);
        setNote(currentNoteItem->note());
    }
    // Si aucune note n'est sélectionnée (plus aucune note dans la liste)
    else
    {
        // On vide la panneau
        setNote(0);
    }
    update();
}
