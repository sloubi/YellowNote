#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include <QtWidgets>

class NoteDialog : public QDialog
{
    Q_OBJECT

    public:
        NoteDialog(QWidget *parent = 0);
        QString note() const;
        QString title() const;

    private:
        QTextEdit *m_note;
        QLineEdit *m_title;
};

#endif // NOTEDIALOG_H
