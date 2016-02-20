#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include <QtWidgets>
#include "note.h"

class NoteDialog : public QWidget
{
    Q_OBJECT

    public:
        NoteDialog(Note *note = 0);
        QString content() const;
        QString title() const;
        Note * note();
        void setContent(const QString & content);
        void setTitle(const QString & content);
        void setNote(Note* note);

    protected:
        void changeEvent(QEvent *event);
        void closeEvent(QCloseEvent *event);
        void save();

    signals:
        void backupRequested(NoteDialog *);
        void deletionRequested(Note *);

    protected slots:
        void handleChanging(const QString & text = "");
        void infos();
        void deleteMe();

    private:
        QTextEdit *m_content;
        QLineEdit *m_title;

        // La note a-t-elle subit un changement depuis le dernier enregistrement
        bool m_changed;

        Note *m_note;
};

#endif // NOTEDIALOG_H
