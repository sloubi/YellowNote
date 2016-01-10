#include "notedialog.h"

NoteDialog::NoteDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Nouvelle note");

    m_title = new QLineEdit;
    m_note = new QTextEdit;
    QPushButton *ok = new QPushButton("Ajouter la note");

    QFormLayout *layout = new QFormLayout;
    layout->addRow("Titre", m_title);
    layout->addRow("Note", m_note);
    layout->addWidget(ok);

    QObject::connect(ok, SIGNAL(clicked()), this, SLOT(accept()));

    setLayout(layout);

    // Donne le focus à la fenêtre (quand elle est créée
    // depuis un raccouri clavier global)
    showNormal();
    raise();
    activateWindow();
}

QString NoteDialog::note() const
{
    return m_note->toPlainText();
}

QString NoteDialog::title() const
{
    return m_title->text();
}
