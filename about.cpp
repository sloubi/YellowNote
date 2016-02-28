#include "about.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLocale>
#include <QPushButton>
#include <QDebug>
#include "sqlutils.h"
#include "oauth2.h"

About::About(QString appVersion, QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle("À propos de YellowNote");

    m_appVersion = appVersion;

    QLabel *image = new QLabel(this);
    image->setPixmap(QPixmap(":/note/logo"));
    image->setStyleSheet("margin-bottom: 8px;");

    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(tabInfos(), "Informations");
    tabWidget->addTab(tabCredits(), "Crédits");
    tabWidget->addTab(tabLicense(), "Licence");
    tabWidget->setStyleSheet("QLabel { padding: 8px; color: #5c5c5c; }");

    QPushButton *close = new QPushButton("&Fermer", this);
    connect(close, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *layoutHeader = new QHBoxLayout();
    layoutHeader->addWidget(image);
    layoutHeader->addWidget(header());

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(layoutHeader);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(close, 0, Qt::AlignRight);
    setLayout(mainLayout);
}

QLabel* About::header()
{
    QLabel *header = new QLabel(this);
    header->setOpenExternalLinks(true);
    header->setText("<font size='6'><b>YellowNote " + m_appVersion + "</b></font><br>"
        "<i>Conservez vos notes facilement !</i><br><br>"
        "Développé par Sloubi<br>"
        "<a href='http://sloubi.eu' style='color: #0066CC;'>http://sloubi.eu</a>");

    return header;
}

QLabel* About::tabInfos()
{
    QDate builtDate = QLocale(QLocale::English).toDate(QString(__DATE__).simplified(), "MMM d yyyy");

    QLabel* infos = new QLabel();
    infos->setAlignment(Qt::AlignTop);
    infos->setText("Compilé le " + builtDate.toString("dd/MM/yyyy") + " à " + QString(__TIME__) + "<br>"
        "Base de données : " + SqlUtils::dbPath() + "<br>"
        "Identifiant du périphérique : " + Oauth2::getMacAddress());

    return infos;
}

QLabel* About::tabCredits()
{
    QString sqliteVersion = SqlUtils::sqlLiteVersion();

    QLabel* credits = new QLabel();
    credits->setAlignment(Qt::AlignTop);
    credits->setOpenExternalLinks(true);
    credits->setText("Qt " + QString(QT_VERSION_STR) + "<br>"
        "SQLite " + sqliteVersion + "<br>"
        "OAuth 2.0 for Qt par <a href='https://github.com/pipacs/o2' style='color: #0066CC;'>pipacs</a><br>"
        "Icônes par <a href='https://icons8.com/' style='color: #0066CC;'>icons8</a></font>");

    return credits;
}

QLabel* About::tabLicense()
{
    QLabel* license = new QLabel("licence");
    license->setAlignment(Qt::AlignTop);
    return license;
}
