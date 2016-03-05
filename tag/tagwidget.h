#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMap>

class TagWidget : public QWidget
{
    Q_OBJECT
public:
    TagWidget(const QString &text);
    static QMap<QString, int> all();
    static int insertInDb(const QString& name);
protected slots:
    void deleteMe();
protected:
    QLabel *m_label;
};

#endif // TAGWIDGET_H
