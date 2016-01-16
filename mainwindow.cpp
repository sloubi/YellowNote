/**
 * Projet créé le 30 décembre 2015
 */

#include <QDebug>
#include "mainwindow.h"

MainWindow::MainWindow(bool *hotkeyLoop) : QMainWindow()
{
    setWindowTitle("YellowNote");

    m_listWidget = new QListWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_listWidget);

    QWidget *central = new QWidget;
    central->setLayout(mainLayout);
    setCentralWidget(central);

    QObject::connect(m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openEditNoteDialog(QListWidgetItem*)));

    createMenus();

    initialize();
    m_hotkeyLoop = hotkeyLoop;
}

void MainWindow::createMenus()
{
    QAction *actionQuit = new QAction("&Quitter", this);
    actionQuit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    QAction *actionNew = new QAction("&Nouvelle note", this);
    actionNew->setIcon(QIcon(":/note/add"));
    actionNew->setIconText("Nouvelle Note");
    actionNew->setShortcut(QKeySequence("Ctrl+N"));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(openNoteDialog()));

    QAction *actionDelete = new QAction("&Supprimer la note", this);
    actionDelete->setIcon(QIcon(":/note/delete"));
    actionDelete->setIconText("Supprimer la note");
    actionDelete->setShortcut(QKeySequence("Suppr"));
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteNote()));

    QAction *actionAbout = new QAction("&À propos de YellowNote", this);
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QMenu *menuNotes = menuBar()->addMenu("&Notes");
    menuNotes->addAction(actionNew);
    menuNotes->addAction(actionDelete);
    menuNotes->addAction(actionQuit);

    QMenu *menuHelp = menuBar()->addMenu("&Aide");
    menuHelp->addAction(actionAbout);

    QToolBar *toolBar = addToolBar("Toolbar");
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addAction(actionNew);
    toolBar->addAction(actionDelete);
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

void MainWindow::close()
{
    // On sort de la boucle infinie des raccourcis globaux
    *m_hotkeyLoop = false;
    qApp->quit();
}

void MainWindow::deleteNote()
{
    QList<QListWidgetItem*> selected = m_listWidget->selectedItems();
    if ( ! selected.empty())
    {
        QListWidgetItem *item = selected.first();
        NoteListWidgetItem *noteItem = static_cast<NoteListWidgetItem*>(item);
        noteItem->note()->deleteInDb();

        m_listWidget->takeItem(m_listWidget->row(item));
    }
}

void MainWindow::about()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("À propos de YellowNote");

    QLabel *image = new QLabel(dialog);
    image->setPixmap(QPixmap(":/note/logo"));

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.exec("SELECT sqlite_version()");
    q.next();
    QString sqliteVersion = q.value(0).toString();

    QDate builtDate = QLocale(QLocale::English).toDate(QString(__DATE__).simplified(), "MMM d yyyy");

    QString aboutString;
    aboutString += "<b>YellowNote 0.1</b><br>";
    aboutString += "Par Sloubi, <a href='http://sloubi.eu'>sloubi.eu</a><br><br>";
    aboutString += "<font color='#5C5C5C'>Compilé le " + builtDate.toString("dd/MM/yyyy") + " à " + QString(__TIME__) + "<br>";
    aboutString += "Qt " + QString(QT_VERSION_STR) + "<br>";
    aboutString += "SQLite " + sqliteVersion + "</font>";

    QLabel *text = new QLabel(dialog);
    text->setText(aboutString);
    text->setOpenExternalLinks(true);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(image);
    layout->addWidget(text);

    QPushButton *close = new QPushButton("&Fermer", this);
    connect(close, SIGNAL(clicked()), dialog, SLOT(accept()));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(layout);
    mainLayout->addWidget(close, 0, Qt::AlignRight);

    dialog->setLayout(mainLayout);
    dialog->show();
}
