#ifndef NOTELABEL_H
#define NOTELABEL_H

#include <QtWidgets>
#include <QString>

class NoteLabel : public QLabel
{
    Q_OBJECT

    public:
        NoteLabel(const QString & title, const QString & text);
        void setTitle(const QString & text);

    private slots:
        void dbClicked();

    private:
        QString m_title;
};

#endif // NOTELABEL_H
