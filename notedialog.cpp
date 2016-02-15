#include "notedialog.h"

NoteDialog::NoteDialog(Note *note) : QWidget()
{
    setWindowTitle(note == 0 ? "Nouvelle note" : note->title());

    m_title = new QLineEdit;
    m_title->setPlaceholderText("Titre");
    m_title->setStyleSheet("border:0; border-bottom:1px solid #ddd; padding: 10px;");

    m_content = new QTextEdit;
    m_content->setPlaceholderText("Commencez à taper votre note");
    m_content->setStyleSheet("border: 0; padding: 10px;background-color: #fff;");

    // Par défaut, la note n'est rattaché à aucune NoteListWidgetItem
    m_itemRow = -1;

    m_changed = false;

    m_note = note;

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

    // Donne le focus à la fenêtre (quand elle est créée
    // depuis un raccouri clavier global)
    showNormal();
    raise();
    activateWindow();

    // Donne le focus sur la note elle-même
    m_content->setFocus();
}

QString NoteDialog::content() const
{
    return m_content->toPlainText();
}

QString NoteDialog::title() const
{
    return m_title->text();
}

int NoteDialog::itemRow() const
{
    return m_itemRow;
}

void NoteDialog::setTitle(const QString & title)
{
    m_title->setText(title);
}

void NoteDialog::setContent(const QString & content)
{
    m_content->setText(content);
}

void NoteDialog::setItemRow(int row)
{
    m_itemRow = row;
}

void NoteDialog::setNote(Note* note)
{
    m_note = note;
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
    event->accept();
}

void NoteDialog::save()
{
    if (m_changed)
    {
        emit saved(this);
        m_changed = false;
    }
}

void NoteDialog::infos()
{
    if (!m_note)
        return;

    QDialog *dialog = new QDialog(this);
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
}
