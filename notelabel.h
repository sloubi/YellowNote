#ifndef NOTELABEL_H
#define NOTELABEL_H

#include <QtWidgets>
#include <QString>
#include "note.h"

class NoteLabel : public QLabel
{
    Q_OBJECT

    public:
        NoteLabel(const QString & title, const QString & content, const QDateTime & updatedAt);
        NoteLabel(const Note & note);

    private:
        void print();

        QString m_title;
        QString m_content;
        QDateTime m_updatedAt;
};

#endif // NOTELABEL_H
