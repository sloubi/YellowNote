#include "connexiondialog.h"

ConnexionDialog::ConnexionDialog(Oauth2 *o2, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Connexion");
    m_user = new QLineEdit;
    m_pass = new QLineEdit;
    m_pass->setEchoMode(QLineEdit::Password);

    m_connexionButton = new QPushButton("Connexion");
    QObject::connect(m_connexionButton, SIGNAL(clicked()), this, SLOT(connexion()));

    QFormLayout *layout = new QFormLayout;
    layout->addRow("Email", m_user);
    layout->addRow("Mot de passe", m_pass);
    layout->addWidget(m_connexionButton);
    setLayout(layout);

    m_o2 = o2;
    connect(m_o2, SIGNAL(linkingFailed()), this, SLOT(error()));
    connect(m_o2, SIGNAL(linkingSucceeded()), this, SLOT(linked()));
}

bool ConnexionDialog::isEmailValid(const QString & email)
{
    QRegExp emailRE("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    emailRE.setCaseSensitivity(Qt::CaseInsensitive);
    emailRE.setPatternSyntax(QRegExp::RegExp);
    return emailRE.exactMatch(email);
}

bool ConnexionDialog::isFormValid()
{
    bool valid = true;
    if (m_user->text().isEmpty() || m_pass->text().isEmpty())
    {
        QMessageBox::warning(this, "Erreur dans le formulaire", "Veuillez remplir tous les champs.");
        valid = false;
    }
    else if (!isEmailValid(m_user->text()))
    {
        QMessageBox::warning(this, "Erreur dans le formulaire", "L'adresse email est invalide.");
        valid = false;
    }

    return valid;
}

void ConnexionDialog::connexion()
{
    if (isFormValid())
    {
        m_connexionButton->setText("Connexion...");
        m_connexionButton->setEnabled(false);

        m_o2->setUsername(m_user->text());
        m_o2->setPassword(m_pass->text());
        m_o2->link();
    }
}

void ConnexionDialog::error()
{
    QString message;

    O2AbstractStore *store = m_o2->store();
    int error = store->value("error").toInt();

    if (error == QNetworkReply::AuthenticationRequiredError)
        message = "Email ou mot de passe incorrect.";
    else if (error == QNetworkReply::ConnectionRefusedError)
        message = "Impossible de contacter le serveur YellowNote. Vérifier votre connexion Internet ou réessayer plus tard.";
    else
        message = "Une erreur est survenue en contactant le serveur YellowNote. Veuillez réessayer plus tard.";

    QMessageBox::critical(this, "Erreur de connexion", message);

    m_connexionButton->setText("Connexion");
    m_connexionButton->setEnabled(true);
}

void ConnexionDialog::linked()
{
    emit connected();
    close();
}
