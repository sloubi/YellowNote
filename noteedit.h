#ifndef NOTEEDIT_H
#define NOTEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QEvent>
#include <QSettings>

class NoteEdit : public QWidget
{
    Q_OBJECT

public:
    NoteEdit(QSettings *settings, QWidget *parent = 0);
    QString content() const;
    QString title() const;
    void setTitle(const QString & title);
    void setContent(const QString & content);
    int contentChars();
    int contentWords();
    int contentLines();
    bool hasChanged();
    void setNoChange();
    void clear();
    void update(const QString& title, const QString& content);

protected slots:
    void handleChanging(const QString & text = "");

signals:
    void lostFocus();

protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

private:
    QLineEdit *m_title;
    QTextEdit *m_content;
    // La note a-t-elle subit un changement depuis le dernier enregistrement
    bool m_changed;
};

#endif // NOTEEDIT_H
