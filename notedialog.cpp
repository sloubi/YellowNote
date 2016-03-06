#include "notedialog.h"
#include "note.h"
#include "mainwindow.h"

NoteDialog::NoteDialog(Note *note) : QWidget()
{
    setWindowTitle(note == 0 ? "Nouvelle note" : note->title());

    // Récupération des options
    m_settings = new QSettings("yellownote.ini", QSettings::IniFormat);

    // Création des champs title et content
    m_noteEdit = new NoteEdit(m_settings, this);

    m_note = note;

    // Assignation du dialog à la note
    attachToNote();

    // Si on est en modification, on remplit les champs
    if (note != 0)
    {
        m_noteEdit->update(note->title(), note->content());
    }

    QAction *actionInfos = new QAction("&Infos", this);
    actionInfos->setIcon(QIcon(":/note/infos"));
    actionInfos->setIconText("Infos");
    connect(actionInfos, SIGNAL(triggered()), this, SLOT(infos()));

    m_actionDelete = new QAction("&Supprimer", this);
    m_actionDelete->setIcon(QIcon(":/note/delete"));
    m_actionDelete->setIconText("Supprimer");
    connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(deleteMe()));

    m_tags = new TagContainer(m_note, this);

    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);

    QToolBar *toolBar = new QToolBar;
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->setIconSize(QSize(24, 24));
    toolBar->addWidget(m_tags);
    toolBar->addWidget(spacerWidget);
    toolBar->addAction(actionInfos);
    toolBar->addAction(m_actionDelete);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(m_noteEdit);
    setLayout(mainLayout);

    // Réassignation de la taille de la fenêtre
    if (m_settings->contains("note/size"))
    {
        resize(m_settings->value("note/size").toSize());
    }

    // Donne le focus à la fenêtre (quand elle est créée
    // depuis un raccouri clavier global)
    setFocus();
}

void NoteDialog::setFocus()
{
    showNormal();
    raise();
    activateWindow();
}

QString NoteDialog::content() const
{
    return m_noteEdit->content();
}

QString NoteDialog::title() const
{
    return m_noteEdit->title();
}

Note * NoteDialog::note()
{
    return m_note;
}

void NoteDialog::setNote(Note* note)
{
    m_note = note;
    attachToNote();
    m_tags->setNote(note);
}

void NoteDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    // Perte du focus
    // (cette méthode fonctionne mieux que de surcharger focusOutEvent)
    if (event->type() == QEvent::ActivationChange && ! this->isActiveWindow())
    {
        save();
    }
}

void NoteDialog::closeEvent(QCloseEvent *event)
{
    save();
    if (m_note)
    {
        delete m_note->noteDialog();
        m_note->setNoteDialog(0);
    }
    event->accept();
}

void NoteDialog::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);

   m_settings->setValue("note/size", size());
}

bool NoteDialog::save()
{
    if (m_noteEdit->hasChanged())
    {
        // Si c'est un ajout
        if (m_note == 0)
        {
            // Création de la note
            Note *note = new Note(title(), content());
            note->save();
            setNote(note);

            // Ajout de la note dans la liste
            emit newNote(note);
        }

        // Modification
        else
        {
            // Mise à jour de la note elle-même
            m_note->setTitle(title());
            m_note->setContent(content());
            m_note->setToSync(true);
            m_note->save();
        }

        // Enregistrement terminée, il n'y a donc plus de changements en attente
        m_noteEdit->setNoChange();
        return true;
    }
    return false;
}

void NoteDialog::infos()
{
    if (!m_note)
    {
        QMessageBox::information(this, "Informations sur la note",
            "Aucune information pour l'instant.<br>La note n'est pas encore enregistrée.");
        return;
    }

    QDialog *dialog = new QDialog(this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    dialog->setWindowTitle("Informations sur la note");

    QLabel *creation = new QLabel(m_note->createdAt().toString("d MMM yyyy à hh:mm:ss"));
    QLabel *update = new QLabel(m_note->updatedAt().toString("d MMM yyyy à hh:mm:ss"));
    QLabel *sync = new QLabel(m_note->toSync() ? "Synchronisation nécessaire" :
        "Synchronisé le " + m_note->syncedAt().toString("d MMM yyyy à hh:mm:ss"));

    QLabel *chars = new QLabel(QString::number(m_noteEdit->contentChars()));
    QLabel *words = new QLabel(QString::number(m_noteEdit->contentWords()));
    QLabel *lines = new QLabel(QString::number(m_noteEdit->contentLines()));

    creation->setAlignment(Qt::AlignRight);
    update->setAlignment(Qt::AlignRight);
    sync->setAlignment(Qt::AlignRight);
    chars->setAlignment(Qt::AlignRight);
    words->setAlignment(Qt::AlignRight);
    lines->setAlignment(Qt::AlignRight);

    QFormLayout *noteLayout = new QFormLayout;
    noteLayout->addRow("Créée le", creation);
    noteLayout->addRow("Mise à jour le", update);
    noteLayout->addRow("Statut", sync);

    QFormLayout *textLayout = new QFormLayout;
    textLayout->addRow("Caractères", chars);
    textLayout->addRow("Mots", words);
    textLayout->addRow("Lignes", lines);

    QGroupBox *noteGroupBox = new QGroupBox("Note");
    noteGroupBox->setLayout(noteLayout);
    QGroupBox *textGroupBox = new QGroupBox("Texte");
    textGroupBox->setLayout(textLayout);

    QPushButton *close = new QPushButton("&Fermer", dialog);
    connect(close, SIGNAL(clicked()), dialog, SLOT(accept()));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(noteGroupBox);
    mainLayout->addWidget(textGroupBox);
    mainLayout->addWidget(close, 0, Qt::AlignRight);

    dialog->setLayout(mainLayout);
    dialog->exec();
}

void NoteDialog::deleteMe()
{
    // close() va lancer l'évènement de fermeture et donc save() va être appelé
    // pour éviter que la note soit sauvegardée, on fait comme si la note n'avait pas été modifiée
    // c'est important dans le cas où on veut supprimer une note qui n'est pas encore enregistrée
    m_noteEdit->setNoChange();

    if (m_note)
    {
        m_note->setDeleteInDb();

        // Suppression du dialog
        m_note->setNoteDialog(0);

        // Suppression de la note dans la liste
        emit deletionRequested(m_note);
    }

    close();
}

// Mise à jour d'affichage
void NoteDialog::update()
{
    if (m_note)
    {
        setWindowTitle(m_note->title());
        m_noteEdit->update(m_note->title(), m_note->content());
        m_tags->update();
    }
    else
    {
        m_noteEdit->clear();
    }
}

void NoteDialog::attachToNote()
{
    if (m_note)
    {
        m_note->setNoteDialog(this);
    }
}
