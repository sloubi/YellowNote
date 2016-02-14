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
#include "hotkeythread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void createMenus();
    void initialize();
    void addNoteLabel(Note *note);
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void addNoteFromDialog(NoteDialog *noteDialog);
    void editNoteFromDialog(NoteDialog *noteDialog);

public slots:
    void openNoteDialog();
    void openEditNoteDialog(QListWidgetItem *item);
    void saveNoteFromDialog(NoteDialog *noteDialog);
    void close();
    void deleteNote();
    void about();
    void sync();
    void onSyncRequestFinished(int id, QNetworkReply::NetworkError error, QByteArray data);
    void handleHotKeyEvent(int modifier, int key);

private:
    QListWidget *m_listWidget;
    QMap<QString, int> m_sharedkeyRows;
    Oauth2 *m_oauth2;
    QSettings *m_o2InternalSettings;
    HotKeyThread *m_hotKeyThread;
};

#endif // MAINWINDOW_H
