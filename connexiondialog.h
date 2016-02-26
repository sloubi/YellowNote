#ifndef CONNEXIONDIALOG_H
#define CONNEXIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include "oauth2.h"

class ConnexionDialog : public QDialog
{
    Q_OBJECT
public:
    ConnexionDialog(Oauth2 *o2, QWidget *parent = 0);
public slots:
    void connexion();
    void error();
    void linked();
signals:
    void connected();
protected:
    bool isEmailValid(const QString & email);
    bool isFormValid();
    QLineEdit *m_user;
    QLineEdit *m_pass;
    QPushButton *m_connexionButton;
    Oauth2 *m_o2;
};

#endif // CONNEXIONDIALOG_H
