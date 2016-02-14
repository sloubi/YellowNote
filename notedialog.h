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

    protected:
        void changeEvent(QEvent *event);
        void closeEvent(QCloseEvent *event);
        void save();

    signals:
        void saved(NoteDialog *);

    protected slots:
        void handleChanging(const QString & text = "");

    private:
        QTextEdit *m_content;
        QLineEdit *m_title;

        // Position du NoteListWidgetItem dans la QListWidget
        int m_itemRow;

        bool m_changed;
};

#endif // NOTEDIALOG_H
