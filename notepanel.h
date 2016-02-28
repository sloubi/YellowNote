#ifndef NOTEPANEL_H
#define NOTEPANEL_H

#include "notedialog.h"
class Note;

class NotePanel : public NoteDialog
{
    Q_OBJECT

public:
    NotePanel();

protected:
    void attachToNote() Q_DECL_OVERRIDE;

protected slots:
    void deleteMe() Q_DECL_OVERRIDE;
    bool save() Q_DECL_OVERRIDE;
};

#endif // NOTEPANEL_H
