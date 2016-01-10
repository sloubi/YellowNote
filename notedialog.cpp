#include "notedialog.h"

NoteDialog::NoteDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Nouvelle note");

    m_title = new QLineEdit;
    m_content = new QTextEdit;
    QPushButton *ok = new QPushButton("Ajouter la note");

    QFormLayout *layout = new QFormLayout;
    layout->addRow("Titre", m_title);
    layout->addRow("Note", m_content);
    layout->addWidget(ok);

    QObject::connect(ok, SIGNAL(clicked()), this, SLOT(accept()));

    setLayout(layout);

    // Donne le focus à la fenêtre (quand elle est créée
    // depuis un raccouri clavier global)
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

void NoteDialog::setTitle(const QString & title)
{
    m_title->setText(title);
}

void NoteDialog::setContent(const QString & content)
{
    m_content->setText(content);
}
