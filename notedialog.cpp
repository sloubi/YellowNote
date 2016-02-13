#include "notedialog.h"

NoteDialog::NoteDialog(Note *note) : QWidget()
{
    setWindowTitle(note == 0 ? "Nouvelle note" : note->title());

    m_title = new QLineEdit;

    m_content = new QTextEdit;

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

    QPushButton *ok = new QPushButton(note == 0 ? "Ajouter la note" : "Enregistrer");

    QFormLayout *layout = new QFormLayout;
    layout->addRow("Titre", m_title);
    layout->addRow("Note", m_content);
    layout->addWidget(ok);

    QObject::connect(ok, SIGNAL(clicked()), this, SLOT(okClicked()));

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

void NoteDialog::okClicked()
{
    // Emet un signal pour la création ou la modification de la note
    // On passe un pointeur vers la fenêtre pour pouvoir récupérer
    // les valeurs du formulaire
    emit saved(this);
    // Ferme la fenêtre
    close();
}
