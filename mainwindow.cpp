/**
 * Projet créé le 30 décembre 2015
 */

#include <QDebug>
#include "mainwindow.h"
#include "connexiondialog.h"
#include "about.h"

MainWindow::MainWindow() : QMainWindow()
{
    setWindowTitle("YellowNote");

    // Récupération des options
    m_settings = new QSettings("yellownote.ini", QSettings::IniFormat);

    createList();
    createPanel();
    createActions();

    initialize();

    // Restore sizes, positions, etc.
    if (m_settings->contains("geometry"))
        restoreGeometry(m_settings->value("geometry").toByteArray());
    if (m_settings->contains("windowState"))
        restoreState(m_settings->value("windowState").toByteArray());
}

void MainWindow::createList()
{
    m_listWidget = new QListWidget();
    m_listWidget->setStyleSheet("QListWidget { background-color: #fff; border: 0; border-right: 1px solid #ddd; padding: 10px; }");
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setUniformItemSizes(true);

    QObject::connect(m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openEditNoteDialog(QListWidgetItem*)));
    QObject::connect(m_listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                     this, SLOT(handleCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));

    setCentralWidget(m_listWidget);
}

void MainWindow::createPanel()
{
    m_notePanel = new NotePanel();
    QObject::connect(m_notePanel, SIGNAL(newNote(Note*)), this, SLOT(addNoteToList(Note*)));
    QObject::connect(m_notePanel, SIGNAL(deletionRequested(Note*)), this, SLOT(deleteNote(Note*)));

    QDockWidget *dock = new QDockWidget("Panneau d'affichage de note", this);
    dock->setObjectName("notePanel");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setTitleBarWidget(new QWidget());
    dock->setWidget(m_notePanel);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::setSyncButtonIcon(int frame)
{
    m_actionSync->setIcon(QIcon(m_movieIconSync->currentPixmap()));

    // Si la sync est terminée, on laisse le gif finir sa boucle
    // puis on l'arrête
    if ( ! m_syncInProgress && frame == 0)
    {
        m_movieIconSync->stop();
    }
}

void MainWindow::createActions()
{
    QAction *actionQuit = new QAction("&Quitter", this);
    actionQuit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    QAction *actionNew = new QAction("&Nouvelle note", this);
    actionNew->setIcon(QIcon(":/note/note"));
    actionNew->setIconText("Nouvelle Note");
    actionNew->setShortcut(QKeySequence("Ctrl+N"));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(openNoteDialog()));

    QAction *actionDelete = new QAction("&Supprimer la note", this);
    actionDelete->setIcon(QIcon(":/note/delete"));
    actionDelete->setIconText("Supprimer la note");
    actionDelete->setShortcut(QKeySequence("Del"));
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedNote()));

    m_actionSync = new QAction("&Synchroniser", this);
    m_actionSync->setIcon(QIcon(":/note/refresh"));
    m_actionSync->setIconText("Synchroniser");
    m_actionSync->setShortcut(QKeySequence("Ctrl+S"));
    connect(m_actionSync, SIGNAL(triggered()), this, SLOT(sync()));

    // Animated icon
    m_movieIconSync = new QMovie(":/note/refresh");
    connect(m_movieIconSync, SIGNAL(frameChanged(int)), this, SLOT(setSyncButtonIcon(int)));

    QAction *actionCheckUpdates = new QAction("&Vérifier les mises à jour", this);
    connect(actionCheckUpdates, SIGNAL(triggered()), this, SLOT(checkUpdates()));

    QAction *actionAbout = new QAction("&À propos de YellowNote", this);
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QMenu *mainMenu = new QMenu(this);
    mainMenu->addAction(m_actionSync);
    mainMenu->addSeparator();
    mainMenu->addAction(actionCheckUpdates);
    mainMenu->addAction(actionAbout);
    mainMenu->addSeparator();
    mainMenu->addAction(actionQuit);

    QToolButton *mainButton = new QToolButton();
    mainButton->setMenu(mainMenu);
    mainButton->setIcon(QIcon(":/note/menu"));
    mainButton->setToolTip("Menu principal de YellowNote");
    mainButton->setPopupMode(QToolButton::InstantPopup);
    mainButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");

    QToolBar *toolBar = addToolBar("Barre d'outils");
    toolBar->setObjectName("toolBar");
    toolBar->setIconSize(QSize(24, 24));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->setMovable(false);
    toolBar->setStyleSheet("QToolBar { border-bottom: 1px solid #ddd; padding: 8px; }");
    toolBar->addAction(actionNew);
    toolBar->addAction(actionDelete);
    toolBar->addAction(m_actionSync);
    toolBar->addWidget(spacer);
    toolBar->addWidget(mainButton);
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

    // Création de la liste
    QList<Note*> notes = Note::loadFromDb();
    if ( ! notes.empty())
    {
        for (int i = 0; i < notes.size(); ++i)
        {
            addNoteToList(notes.at(i));
        }
    }

    // Sélection de la première note de la liste
    m_listWidget->setCurrentItem(m_listWidget->item(0));

    // Réassignation de la taille de la fenêtre
    if (m_settings->contains("size"))
    {
        resize(m_settings->value("size").toSize());
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

    QSettings *o2Settings = new QSettings("oauth.ini", QSettings::IniFormat);
    O2SettingsStore *settingsStore = new O2SettingsStore(o2Settings, m_o2InternalSettings->value("encryption_key").toString());
    // Set the store before starting OAuth, i.e before calling link()
    m_oauth2->setStore(settingsStore);

    QObject::connect(m_oauth2, SIGNAL(refreshFinished(QNetworkReply::NetworkError)),
                     this, SLOT(onRefreshTokenFinished(QNetworkReply::NetworkError)));
}

void MainWindow::openNoteDialog()
{
    NoteDialog *dialog = new NoteDialog();
    dialog->show();

    QObject::connect(dialog, SIGNAL(newNote(Note*)), this, SLOT(addNoteToList(Note*)));
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

    QObject::connect(dialog, SIGNAL(newNote(Note*)), this, SLOT(addNoteToList(Note*)));
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    beforeClose();

    // On laisse la fenêtre se fermer
    event->accept();
}

void MainWindow::close()
{
    beforeClose();
    qApp->quit();
}

void MainWindow::beforeClose()
{
    // On ferme le thread des raccourcis clavier
    m_hotKeyThread->stop();

    // On enregistre la taille de la fenêtre, la position du panneau etc.
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
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

        noteItem->setNote(0);
        m_listWidget->takeItem(m_listWidget->row(item));
    }
}

void MainWindow::deleteNote(Note *note)
{
    if (note)
    {
        note->setDeleteInDb();
        m_notes.remove(note->sharedKey());

        note->item()->setNote(0);
        m_listWidget->takeItem(m_listWidget->row(note->item()));
    }
}

void MainWindow::about()
{
    About *about = new About(QString(YELLOWNOTE_VERSION), this);
    about->exec();
}

void MainWindow::sync()
{
    if (m_oauth2->linked())
    {
        m_syncInProgress = true;
        m_movieIconSync->start();

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

        // On indique que toutes les notes envoyées vers le serveur ont été synchronisées
        foreach (Note *note, m_notes)
        {
            if (note->toSync())
            {
                note->setSyncedNow();
            }
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
                if (obj["to_delete"].toString().toInt())
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
                        note->setSyncedNow();

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
                        note->addToDb(false);
                        note->setSyncedNow();

                        // Mise à jour de l'affichage
                        addNoteToList(note);
                    }
                }
            }
        }
        else
        {
            QMessageBox::critical(this, "Erreur de connexion", "Une erreur est survenue en contactant le serveur YellowNote. Veuillez réessayer plus tard.");
        }
    }

    m_syncInProgress = false;
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

void MainWindow::onRefreshTokenFinished(QNetworkReply::NetworkError error)
{
    // Si il y a eu une erreur lors de la demande d'un refresh token
    // c'est problablement qu'il est expiré
    if (error != QNetworkReply::NoError)
    {
        // On demande donc la reconnexion
        QMessageBox::warning(this, "Connexion impossible",
            "La connexion a expirée. Vous devez vous reconnecter.");
        ConnexionDialog dialog(m_oauth2, this);
        QObject::connect(&dialog, SIGNAL(connected()), this, SLOT(sync()));
        dialog.exec();
    }
}

void MainWindow::handleCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (current)
    {
        NoteListWidgetItem *currentNoteItem = static_cast<NoteListWidgetItem*>(current);
        // On indique que la note est maintenant affichée sur le panneau
        currentNoteItem->note()->setNotePanel(m_notePanel);

        // Mise à jour du panneau avec la note sélectionné
        m_notePanel->setNote(currentNoteItem->note());
        m_notePanel->update();
    }
    // Si aucune note n'est sélectionnée (plus aucune note dans la liste)
    else
    {
        // On vide la panneau
        m_notePanel->setNote(0);
        m_notePanel->update();
    }

    if (previous)
    {
        NoteListWidgetItem *previousNoteItem = static_cast<NoteListWidgetItem*>(previous);
        if (previousNoteItem->note())
            previousNoteItem->note()->setNotePanel(0);
    }
}
