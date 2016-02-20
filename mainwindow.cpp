/**
 * Projet créé le 30 décembre 2015
 */

#include <QDebug>
#include "mainwindow.h"
#include "connexiondialog.h"

MainWindow::MainWindow() : QMainWindow()
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
    actionDelete->setShortcut(QKeySequence("Del"));
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedNote()));

    QAction *actionSync = new QAction("&Synchroniser", this);
    actionSync->setIcon(QIcon(":/note/sync"));
    actionSync->setIconText("Synchroniser");
    actionSync->setShortcut(QKeySequence("Ctrl+S"));
    connect(actionSync, SIGNAL(triggered()), this, SLOT(sync()));

    QAction *actionCheckUpdates = new QAction("&Vérifier les mises à jour", this);
    connect(actionCheckUpdates, SIGNAL(triggered()), this, SLOT(checkUpdates()));

    QAction *actionAbout = new QAction("&À propos de YellowNote", this);
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QMenu *menuNotes = menuBar()->addMenu("&Notes");
    menuNotes->addAction(actionNew);
    menuNotes->addAction(actionDelete);
    menuNotes->addAction(actionSync);
    menuNotes->addAction(actionQuit);

    QMenu *menuHelp = menuBar()->addMenu("&Aide");
    menuHelp->addAction(actionCheckUpdates);
    menuHelp->addAction(actionAbout);

    QToolBar *toolBar = addToolBar("Toolbar");
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addAction(actionNew);
    toolBar->addAction(actionDelete);
    toolBar->addAction(actionSync);
}

void MainWindow::initialize()
{
    // Thread des raccourcis clavier globaux
    m_hotKeyThread = new HotKeyThread(this);

    QObject::connect(m_hotKeyThread, SIGNAL(hotKeyEvent(int, int)),
        this, SLOT(handleHotKeyEvent(int, int)), Qt::QueuedConnection);

    m_hotKeyThread->start();

    // Initialise la connexion à la base de données
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("notes.db");

    QList<Note*> notes = Note::loadFromDb();
    if ( ! notes.empty())
    {
        for (int i = 0; i < notes.size(); ++i)
        {
            addNoteToList(notes.at(i));
        }
    }

    // Récupération de la configuration du serveur oAuth2
    m_o2InternalSettings = new QSettings(":/config/oauth.ini", QSettings::IniFormat);

    // Paramétrage pour les appels à l'API
    m_oauth2 = new Oauth2();
    m_oauth2->setClientId(m_o2InternalSettings->value("client_id").toString());
    m_oauth2->setClientSecret(m_o2InternalSettings->value("client_secret").toString());
    m_oauth2->setGrantFlow(O2::GrantFlowResourceOwnerPasswordCredentials);
    m_oauth2->setTokenUrl(m_o2InternalSettings->value("token_url").toString());
    m_oauth2->setRefreshTokenUrl(m_o2InternalSettings->value("refresh_token_url").toString());

    QSettings *o2Settings = new QSettings("Sloubi", "oAuth2");
    O2SettingsStore *settingsStore = new O2SettingsStore(o2Settings, m_o2InternalSettings->value("encryption_key").toString());
    // Set the store before starting OAuth, i.e before calling link()
    m_oauth2->setStore(settingsStore);
}

void MainWindow::openNoteDialog()
{
    NoteDialog *dialog = new NoteDialog();
    dialog->show();

    QObject::connect(dialog, SIGNAL(backupRequested(NoteDialog*)), this, SLOT(saveNoteFromDialog(NoteDialog*)));
    QObject::connect(dialog, SIGNAL(deletionRequested(Note*)), this, SLOT(deleteNote(Note*)));
}

void MainWindow::openEditNoteDialog(QListWidgetItem *item)
{
    NoteListWidgetItem *noteItem = static_cast<NoteListWidgetItem*>(item);

    // On vérifie que le dialog n'est pas déjà ouvert
    if (noteItem->note()->noteDialog() != 0)
    {
        noteItem->note()->noteDialog()->setFocus();
        return;
    }

    NoteDialog *dialog = new NoteDialog(noteItem->note());
    dialog->show();

    QObject::connect(dialog, SIGNAL(backupRequested(NoteDialog*)), this, SLOT(saveNoteFromDialog(NoteDialog*)));
    QObject::connect(dialog, SIGNAL(deletionRequested(Note*)), this, SLOT(deleteNote(Note*)));
}

void MainWindow::addNoteToList(Note *note)
{
    // Création du label
    NoteLabel *label = new NoteLabel(note);

    // Création de l'item de la liste, ajout à la liste, assignation du label
    NoteListWidgetItem *item = new NoteListWidgetItem(m_listWidget);
    item->setSizeHint(label->minimumSizeHint());
    m_listWidget->addItem(item);
    m_listWidget->setItemWidget(item, label);

    // Enregistrement de la note dans notre tableau principal
    m_notes[note->sharedKey()] = note;

    // Rattachement de la note à l'item et inversement
    item->setNote(note);
    note->setItem(item);
}

void MainWindow::saveNoteFromDialog(NoteDialog *noteDialog)
{
    // Si c'est un ajout
    if (noteDialog->note() == 0)
    {
       addNoteFromDialog(noteDialog);
    }

    // Modification
    else
    {
        editNoteFromDialog(noteDialog);
    }
}

Note* MainWindow::addNoteFromDialog(NoteDialog *noteDialog)
{
    // On ne crée pas la note si elle n'a pas encore de contenu
    if (noteDialog->title().isEmpty() && noteDialog->content().isEmpty())
        return 0;

    Note *note = new Note(noteDialog->title(), noteDialog->content());
    note->addToDb();
    addNoteToList(note);

    // On rattache la Note au dialog
    noteDialog->setNote(note);

    return note;
}

void MainWindow::editNoteFromDialog(NoteDialog *noteDialog)
{
    noteDialog->note()->setTitle(noteDialog->title());
    noteDialog->note()->setContent(noteDialog->content());
    noteDialog->note()->editInDb();

    noteDialog->note()->item()->update();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // On ferme le thread des raccourcis clavier
    m_hotKeyThread->stop();

    // On laisse la fenêtre se fermer
    event->accept();
}

void MainWindow::close()
{
    // On ferme le thread des raccourcis clavier
    m_hotKeyThread->stop();

    qApp->quit();
}

void MainWindow::deleteSelectedNote()
{
    QList<QListWidgetItem*> selected = m_listWidget->selectedItems();
    if ( ! selected.empty())
    {
        QListWidgetItem *item = selected.first();
        NoteListWidgetItem *noteItem = static_cast<NoteListWidgetItem*>(item);

        noteItem->note()->setDeleteInDb();
        m_notes.remove(noteItem->note()->sharedKey());
        m_listWidget->takeItem(m_listWidget->row(item));
    }
}

void MainWindow::deleteNote(Note *note)
{
    if (note)
    {
        note->setDeleteInDb();
        m_notes.remove(note->sharedKey());
        m_listWidget->takeItem(m_listWidget->row(note->item()));
    }
}

void MainWindow::about()
{
    QDialog *dialog = new QDialog(this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
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
    aboutString += "<b>YellowNote " + QString(YELLOWNOTE_VERSION) + "</b><br>";
    aboutString += "Par Sloubi, <a href='http://sloubi.eu'>sloubi.eu</a><br><br>";
    aboutString += "<font color='#5C5C5C'>Compilé le " + builtDate.toString("dd/MM/yyyy") + " à " + QString(__TIME__) + "<br>";
    aboutString += "Qt " + QString(QT_VERSION_STR) + "<br>";
    aboutString += "SQLite " + sqliteVersion + "<br>";
    aboutString += "OAuth 2.0 for Qt by <a href='https://github.com/pipacs/o2'>pipacs</a></font>";

    QLabel *text = new QLabel(dialog);
    text->setText(aboutString);
    text->setOpenExternalLinks(true);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(image);
    layout->addWidget(text);

    QPushButton *close = new QPushButton("&Fermer", dialog);
    connect(close, SIGNAL(clicked()), dialog, SLOT(accept()));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(layout);
    mainLayout->addWidget(close, 0, Qt::AlignRight);

    dialog->setLayout(mainLayout);
    dialog->exec();
}

void MainWindow::sync()
{
    if (m_oauth2->linked())
    {
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        O2Requestor *requestor = new O2Requestor(manager, m_oauth2);

        QUrl url(m_o2InternalSettings->value("sync_url").toString());

        QUrlQuery params;
        params.addQueryItem("notes", Note::getJsonNotesToSync());

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        requestor->post(request, params.toString(QUrl::FullyEncoded).toUtf8());

        QObject::connect(requestor, SIGNAL(finished(int, QNetworkReply::NetworkError, QByteArray)),
                         this, SLOT(onSyncRequestFinished(int, QNetworkReply::NetworkError, QByteArray)));
    }
    else
    {
        ConnexionDialog dialog(m_oauth2, this);
        QObject::connect(&dialog, SIGNAL(connected()), this, SLOT(sync()));
        dialog.exec();
    }
}

void MainWindow::onSyncRequestFinished(int id, QNetworkReply::NetworkError error, QByteArray data)
{
    if (error == QNetworkReply::AuthenticationRequiredError)
    {
        QMessageBox::critical(this, "Erreur d'authentification", "Session expirée. Vous devez vous reconnecter.");
    }
    else if (error == QNetworkReply::ConnectionRefusedError)
    {
        QMessageBox::critical(this, "Erreur de connexion", "Impossible de contacter le serveur YellowNote. Vérifier votre connexion Internet ou réessayer plus tard.");
    }
    else if (error == QNetworkReply::NoError)
    {
        // Maintenant qu'on a prévenu le serveur que les notes allaient être supprimées,
        // Suppression réelles des notes dans la db local
        Note::deleteInDb();

        // Passage de toutes les notes en to_sync = 0 dans la db local
        Note::setToSyncOffInDb();
        foreach (Note *note, m_notes)
        {
            note->setToSync(false);
            note->setSyncedAt(SqlUtils::getNow());
        }

        QJsonDocument d = QJsonDocument::fromJson(data);
        if (d.isArray())
        {
            QJsonArray jsonArray = d.array();
            foreach (const QJsonValue &value, jsonArray)
            {
                QJsonObject obj = value.toObject();
                QString sharedKey = obj["shared_key"].toString();

                // La note a été supprimée sur le serveur, on la supprime aussi ici
                if (obj["to_delete"].toBool())
                {
                    Note::deleteInDb(sharedKey);
                }
                // Récupération des notes du serveur
                else
                {
                    // Modification d'une note déjà existante
                    if (Note::exists(sharedKey))
                    {
                        // Récupération de la note dans la liste
                        Note *note = m_notes[sharedKey];

                        // Assignation des nouvelles valeurs, MAJ en base
                        note->setTitle(obj["title"].toString());
                        note->setContent(obj["content"].toString());
                        note->setUpdatedAt(obj["updated_at"].toString());
                        note->editInDb(false);

                        // Mise à jour de l'affichage
                        note->item()->update();
                    }
                    // Nouvelle note
                    else
                    {
                        Note *note = new Note(
                            obj["title"].toString(),
                            obj["content"].toString(),
                            obj["shared_key"].toString()
                        );
                        note->setCreatedAt(obj["created_at"].toString());
                        note->setUpdatedAt(obj["updated_at"].toString());
                        note->setToSync(false);
                        note->addToDb();
                        addNoteToList(note);
                    }
                }
            }
        }
        else
        {
            QMessageBox::critical(this, "Erreur de connexion", "Une erreur est survenue en contactant le serveur YellowNote. Veuillez réessayer plus tard.");
            return;
        }
    }
}

void MainWindow::handleHotKeyEvent(int modifier, int key)
{
    if (modifier == MOD_WIN && (key == KEY_Z || key == KEY_N || key == KEY_Y))
        openNoteDialog();
}

void MainWindow::checkUpdates()
{
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl("http://yellownote.sloubi.eu/version.txt")));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QString lastVersion = reply->readAll();

    if (QString(YELLOWNOTE_VERSION) != lastVersion)
    {
        QMessageBox::information(this, "Vérification des mises à jour",
            "Une nouvelle version de YellowNote est disponible ! La version <b>" + lastVersion + "</b> "
            "est disponible sur <a href='http://yellownote.sloubi.eu'>yellownote.sloubi.eu</a>.<br><br>"
            "Vous pouvez télécharger cette version en utilisant le lien suivant :<br>"
            "<a href='https://github.com/sloubi/YellowNote-Qt/releases'>https://github.com/sloubi/YellowNote-Qt/releases</a>");
    }
    else
    {
        QMessageBox::information(this, "Vérification des mises à jour", "Vous avez déjà la dernière version de YellowNote.");
    }
}
