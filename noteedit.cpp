#include "noteedit.h"
#include <QVBoxLayout>
#include <QDebug>

NoteEdit::NoteEdit(QSettings *settings, QWidget *parent) : QWidget(parent)
{
    m_changed = false;

    QString fontFamily = settings->value("note/fontfamily", "Segoe UI").toString();
    int fontSize = settings->value("note/fontsize", 9).toInt();

    m_title = new QLineEdit;
    m_title->setPlaceholderText("Titre");
    m_title->setStyleSheet("border:0; border-bottom:1px solid #ddd; padding: 8px;");
    m_title->setFont(QFont(fontFamily, fontSize + 1));

    m_content = new QTextEdit;
    m_content->setPlaceholderText("Commencez à taper votre note");
    m_content->setStyleSheet("QTextEdit { border: 0; }");
    m_content->document()->setDocumentMargin(10);
    m_content->setAcceptRichText(false);
    m_content->setFont(QFont(fontFamily, fontSize));

    // Pour que la zone de texte s'étire aussi quand la fenêtre est redimensionnée
    QSizePolicy policy = m_content->sizePolicy();
    policy.setVerticalStretch(1);
    m_content->setSizePolicy(policy);

    m_title->installEventFilter(this);
    m_content->installEventFilter(this);

    connect(m_title, SIGNAL(textEdited(QString)), this, SLOT(handleChanging(QString)));
    connect(m_content, SIGNAL(textChanged()), this, SLOT(handleChanging()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_content);
    setLayout(mainLayout);

    // Donne le focus sur le champ note
    m_content->setFocus();
}

bool NoteEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusOut) {
        emit lostFocus();
    }

    return QObject::eventFilter(obj, event);
}

QString NoteEdit::content() const
{
    return m_content->toPlainText();
}

QString NoteEdit::title() const
{
    return m_title->text();
}

void NoteEdit::setTitle(const QString & title)
{
    m_title->setText(title);
}

void NoteEdit::setContent(const QString & content)
{
    m_content->setText(content);
}

int NoteEdit::contentChars()
{
    return content().size();
}

int NoteEdit::contentWords()
{
    return content().split(QRegExp("(\\s|\\n|\\r)+"), QString::SkipEmptyParts).count();
}

int NoteEdit::contentLines()
{
    return m_content->document()->lineCount();
}

bool NoteEdit::hasChanged()
{
    return m_changed;
}

void NoteEdit::handleChanging(const QString & text)
{
    m_changed = true;
}

void NoteEdit::setNoChange()
{
    m_changed = false;
}

void NoteEdit::clear()
{
    m_title->clear();
    m_content->clear();
}

void NoteEdit::update(const QString& title, const QString& content)
{
    setTitle(title);

    // Le signal textEdited() n'existe pas pour les QTextEdit
    // On bloque donc carrément le signal, sinon le slot handleChanging va être
    // appelé, et il va passer la note comme étant modifiée, ce qui n'est pas le cas,
    // cette fonction étant utilisée pour mettre à jour l'affichage
    m_content->blockSignals(true);
    setContent(content);
    m_content->blockSignals(false);
}
