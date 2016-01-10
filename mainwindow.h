#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtWidgets>
#include <QtSql>
#include "notelistwidgetitem.h"
#include "notelabel.h"
#include "notedialog.h"
#include "note.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(bool *hotkeyLoop);

protected:
    void initialize();
    void addNoteLabel(const Note &note);
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

public slots:
    void openNoteDialog();

private slots:
    void addNoteFromDialog();
    void save();
    void openEditNoteDialog(QListWidgetItem *item);

private:
    QListWidget *m_listWidget;
    NoteDialog *m_dialog;
    QList<Note> m_notes;
    bool *m_hotkeyLoop;

};

#endif // MAINWINDOW_H
