#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtWidgets>
#include <QtSql>
#include <QNetworkReply>
#include "oauth2.h"
#include "o2/o2settingsstore.h"
#include "o2/o2requestor.h"
#include "notelistwidgetitem.h"
#include "notelabel.h"
#include "notedialog.h"
#include "note.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(bool *hotkeyLoop);

protected:
    void createMenus();
    void initialize();
    void addNoteLabel(Note *note);
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

public slots:
    void openNoteDialog();
    void addNoteFromDialog(NoteDialog *noteDialog);
    void openEditNoteDialog(QListWidgetItem *item);
    void editNoteFromDialog(NoteDialog *noteDialog);
    void close();
    void deleteNote();
    void about();
    void sync();
    void test(int id, QNetworkReply::NetworkError error, QByteArray data);

private:
    QListWidget *m_listWidget;
    QMap<QString, int> m_sharedkeyRows;
    Oauth2 *m_oauth2;
    bool *m_hotkeyLoop;
    QSettings *m_o2InternalSettings;
};

#endif // MAINWINDOW_H
