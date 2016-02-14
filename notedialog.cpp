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

    QAction *actionDelete = new QAction("&Supprimer", this);
    actionDelete->setIcon(QIcon(":/note/delete"));
    actionDelete->setIconText("Supprimer");

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
        qDebug() << "save";
        emit saved(this);
        m_changed = false;
    }
}
