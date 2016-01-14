#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtWidgets>
#include <QtSql>
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

private:
    QListWidget *m_listWidget;
    bool *m_hotkeyLoop;

};

#endif // MAINWINDOW_H
