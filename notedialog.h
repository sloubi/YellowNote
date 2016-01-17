#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include <QtWidgets>
#include "note.h"

class NoteDialog : public QWidget
{
    Q_OBJECT

    public:
        NoteDialog(Note * = 0);
        QString content() const;
        QString title() const;
        int itemRow() const;
        void setContent(const QString & content);
        void setTitle(const QString & content);
        void setItemRow(int row);

    signals:
        void saved(NoteDialog *);

    private slots:
        void okClicked();

    private:
        QTextEdit *m_content;
        QLineEdit *m_title;

        // Position du NoteListWidgetItem dans la QListWidget
        int m_itemRow;
};

#endif // NOTEDIALOG_H
