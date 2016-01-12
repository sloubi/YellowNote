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

    m_listWidget = new QListWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(m_listWidget);
    setLayout(mainLayout);

    QObject::connect(addButton, SIGNAL(clicked()), this, SLOT(openNoteDialog()));
    QObject::connect(m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openEditNoteDialog(QListWidgetItem*)));

    initialize();
    m_hotkeyLoop = hotkeyLoop;
}

void MainWindow::initialize()
{
    // Initialise la connexion à la base de données
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("notes.db");

    QList<Note> notes = Note::loadFromDb();
    if ( ! notes.empty())
    {
        for (int i = 0; i < notes.size(); ++i)
        {
            Note *note = new Note();
            *note = notes.at(i);
            addNoteLabel(note);
        }
    }
}

void MainWindow::openNoteDialog()
{
    NoteDialog *dialog = new NoteDialog(this);
    dialog->show();

    QObject::connect(dialog, SIGNAL(saved(NoteDialog*)), this, SLOT(addNoteFromDialog(NoteDialog*)));
}

void MainWindow::openEditNoteDialog(QListWidgetItem *item)
{
    NoteListWidgetItem *noteItem = static_cast<NoteListWidgetItem*>(item);

    NoteDialog *dialog = new NoteDialog(this, noteItem->note());
    dialog->setItemRow(m_listWidget->row(noteItem));
    dialog->show();

    QObject::connect(dialog, SIGNAL(saved(NoteDialog*)), this, SLOT(editNoteFromDialog(NoteDialog*)));
}

void MainWindow::addNoteLabel(Note *note)
{
    NoteLabel *label = new NoteLabel(note->title(), note->content());

    NoteListWidgetItem *item = new NoteListWidgetItem(m_listWidget);
    item->setSizeHint(label->minimumSizeHint());
    m_listWidget->addItem(item);
    m_listWidget->setItemWidget(item, label);

    item->setNote(note);
}

void MainWindow::addNoteFromDialog(NoteDialog *noteDialog)
{
    Note *note = new Note(noteDialog->title(), noteDialog->content());
    addNoteLabel(note);
    note->addToDb();
}

void MainWindow::editNoteFromDialog(NoteDialog *noteDialog)
{
    NoteLabel *label = new NoteLabel(noteDialog->title(), noteDialog->content());

    QListWidgetItem *item = m_listWidget->item(noteDialog->itemRow());
    NoteListWidgetItem *noteItem = static_cast<NoteListWidgetItem*>(item);
    noteItem->setSizeHint(label->minimumSizeHint());
    m_listWidget->setItemWidget(item, label);

    noteItem->note()->setTitle(noteDialog->title());
    noteItem->note()->setContent(noteDialog->content());

    noteItem->note()->editInDb();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // On sort de la boucle infinie des raccourcis globaux
    *m_hotkeyLoop = false;

    // On laisse la fenêtre se fermer
    event->accept();
}
