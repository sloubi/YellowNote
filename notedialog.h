#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include <QtWidgets>

class NoteDialog : public QDialog
{
    Q_OBJECT

    public:
        NoteDialog(QWidget *parent = 0);
        QString content() const;
        QString title() const;
        void setContent(const QString & content);
        void setTitle(const QString & content);

    private:
        QTextEdit *m_content;
        QLineEdit *m_title;
};

#endif // NOTEDIALOG_H
