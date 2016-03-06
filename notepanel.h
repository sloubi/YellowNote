#ifndef NOTEPANEL_H
#define NOTEPANEL_H

#include "notelistwidgetitem.h"
#include "notedialog.h"
class Note;

class NotePanel : public NoteDialog
{
    Q_OBJECT

public:
    NotePanel();
    static NotePanel* getInstance();

protected:
    void attachToNote() Q_DECL_OVERRIDE;

public slots:
    void handleNoteChanged(QListWidgetItem* current, QListWidgetItem* previous);

protected slots:
    void deleteMe() Q_DECL_OVERRIDE;

private:
    static NotePanel *m_instance;
};

#endif // NOTEPANEL_H
