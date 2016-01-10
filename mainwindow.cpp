/**
 * Projet créé le 30 décembre 2015
 */

#include <QDebug>
#include "mainwindow.h"

MainWindow::MainWindow(bool *hotkeyLoop) : QWidget()
{
    setWindowTitle("YellowNote");

    QPushButton *addButton = new QPushButton("Ajouter une note");
    QPushButton *saveButton = new QPushButton("Enregistrer");

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(saveButton);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    m_notesWidget = new QWidget(scrollArea);
    m_notesWidget->setGeometry(0, 0, 200, 80);

    QVBoxLayout *scrolledLayout = new QVBoxLayout();
    m_notesWidget->setLayout(scrolledLayout);

    scrollArea->setWidget(m_notesWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(scrollArea);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    QObject::connect(addButton, SIGNAL(clicked()), this, SLOT(openNoteDialog()));
    QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));

    initialize();
    m_hotkeyLoop = hotkeyLoop;
}

void MainWindow::initialize()
{
    // Initialise la connexion à la base de données
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("notes.db");

    m_notes = Note::loadFromDb();
    if (m_notes.empty())
    {
        QMessageBox::warning(this, "Notes introuvable", "Le fichier de stockage des notes est introuvable.");
    }

    for (int i = 0; i < m_notes.size(); ++i)
    {
        addNoteLabel(m_notes.at(i));
    }
}

void MainWindow::openNoteDialog()
{
    m_dialog = new NoteDialog(this);
    m_dialog->show();

    QObject::connect(m_dialog, SIGNAL(accepted()), this, SLOT(addNoteFromDialog()));
}

void MainWindow::addNoteLabel(const Note & note)
{
    NoteLabel *label = new NoteLabel(note.title(), note.content());
    m_notesWidget->layout()->addWidget(label);
}

void MainWindow::addNoteFromDialog()
{
    Note note(m_dialog->title(), m_dialog->note());
    m_notes.append(note);
    addNoteLabel(note);
    note.addToDb();
}

void MainWindow::save()
{
    Note::writeToFile(m_notes);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // On sort de la boucle infinie des raccourcis globaux
    *m_hotkeyLoop = false;

    // On laisse la fenêtre se fermer
    event->accept();
}
