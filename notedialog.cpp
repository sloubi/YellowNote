#include "notedialog.h"
#include "note.h"
#include "mainwindow.h"

NoteDialog::NoteDialog(Note *note) : QWidget()
{
    setWindowTitle(note == 0 ? "Nouvelle note" : note->title());

    m_title = new QLineEdit;
    m_title->setPlaceholderText("Titre");
    m_title->setStyleSheet("border:0; border-bottom:1px solid #ddd; padding: 10px;");

    m_content = new QTextEdit;
    m_content->setPlaceholderText("Commencez à taper votre note");
    m_content->setStyleSheet("QTextEdit { border: 0; }");
    m_content->document()->setDocumentMargin(10);
    m_content->setAcceptRichText(false);

    m_changed = false;

    m_note = note;
    if (m_note)
        m_note->setNoteDialog(this);

    // Pour que la zone de texte s'étire aussi quand la fenêtre est redimensionnée
    QSizePolicy policy = m_content->sizePolicy();
    policy.setVerticalStretch(1);
    m_content->setSizePolicy(policy);

    // Si on est en modification, on remplit les champs
    if (note != 0)
    {
        setTitle(note->title());
        setContent(note->content());
    }

    QAction *actionInfos = new QAction("&Infos", this);
    actionInfos->setIcon(QIcon(":/note/infos"));
    actionInfos->setIconText("Infos");
    connect(actionInfos, SIGNAL(triggered()), this, SLOT(infos()));

    QAction *actionDelete = new QAction("&Supprimer", this);
    actionDelete->setIcon(QIcon(":/note/delete"));
    actionDelete->setIconText("Supprimer");
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteMe()));

    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);

    QToolBar *toolBar = new QToolBar;
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addWidget(spacerWidget);
    toolBar->addAction(actionInfos);
    toolBar->addAction(actionDelete);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_content);
    setLayout(mainLayout);

    connect(m_title, SIGNAL(textChanged(QString)), this, SLOT(handleChanging(QString)));
    connect(m_content, SIGNAL(textChanged()), this, SLOT(handleChanging()));

    // Récupération des options
    m_settings = new QSettings("yellownote.ini", QSettings::IniFormat);

    // Réassignation de la taille de la fenêtre
    if (m_settings->contains("width") && m_settings->contains("height"))
    {
        resize(m_settings->value("width").toInt(), m_settings->value("height").toInt());
    }

    // Donne le focus à la fenêtre (quand elle est créée
    // depuis un raccouri clavier global)
    setFocus();

    // Donne le focus sur le champ note
    m_content->setFocus();
}

void NoteDialog::setFocus()
{
    showNormal();
    raise();
    activateWindow();
}

QString NoteDialog::content() const
{
    return m_content->toPlainText();
}

QString NoteDialog::title() const
{
    return m_title->text();
}

Note * NoteDialog::note()
{
    return m_note;
}

void NoteDialog::setTitle(const QString & title)
{
    m_title->setText(title);
}

void NoteDialog::setContent(const QString & content)
{
    m_content->setText(content);
}

void NoteDialog::setNote(Note* note)
{
    m_note = note;
    m_note->setNoteDialog(this);
}

void NoteDialog::handleChanging(const QString & text)
{
    m_changed = true;
}

void NoteDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    // Perte du focus
    // (cette méthode fonctionne mieux que de surcharger focusOutEvent)
    if (event->type() == QEvent::ActivationChange && ! this->isActiveWindow())
    {
        save();
        m_changed = false;
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

   m_settings->setValue("width", size().width());
   m_settings->setValue("height", size().height());
}

void NoteDialog::save()
{
    if (m_changed)
    {
        emit backupRequested(this);
        m_title->setText(m_note->title());
        setWindowTitle(m_note->title());
        m_changed = false;
    }
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

    QLabel *chars = new QLabel(QString::number(content().size()));
    QLabel *words = new QLabel(QString::number(content().split(QRegExp("(\\s|\\n|\\r)+"), QString::SkipEmptyParts).count()));
    QLabel *lines = new QLabel(QString::number(m_content->document()->lineCount()));

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
    // signal de suppression
    if (m_note)
    {
        emit deletionRequested(m_note);
    }

    // close() va lancer l'évènement de fermeture et donc save() va être appelé
    // pour éviter que la note soit sauvegardée, on fait comme si la note n'avait pas été modifiée
    // c'est important dans le cas où on veut supprimer une note qui n'est pas encore enregistrée
    m_changed = false;

    close();
}
