#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QLabel>

class About : public QDialog
{
public:
    About(QString appVersion, QWidget *parent = 0);

protected:
    QLabel* header();
    QLabel* tabInfos();
    QLabel* tabCredits();
    QLabel* tabLicense();

    QString m_appVersion;
};

#endif // ABOUT_H
