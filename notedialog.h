#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include <QtWidgets>
#include "noteedit.h"
#include "tag/tagcontainer.h"
class Note;

class NoteDialog : public QWidget
{
    Q_OBJECT

    public:
        NoteDialog(Note *note = 0);
        QString content() const;
        QString title() const;
        Note * note();
        void setNote(Note* note);
        void setFocus();
        void update();

    protected:
        void changeEvent(QEvent *event);
        void closeEvent(QCloseEvent *event);
        void resizeEvent(QResizeEvent* event);
        virtual void attachToNote();

    signals:
        void newNote(Note *);
        void deletionRequested(Note *);

    public slots:
        void deleteMe();

    protected slots:
        void infos();
        bool save();

    protected:
        NoteEdit *m_noteEdit;
        QSettings *m_settings;
        Note *m_note;
        QAction *m_actionDelete;
        TagContainer *m_tags;
};

#endif // NOTEDIALOG_H
