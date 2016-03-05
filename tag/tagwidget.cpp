#include "tagwidget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QtSql>
#include "sqlutils.h"

TagWidget::TagWidget(const QString& text) : QWidget()
{
    setStyleSheet("QLabel, QPushButton { background: #ddd; color: #466394; }"
                  "QLabel { padding: 0 2px 2px;}"
                  "QPushButton { border: 0; padding: 0 4px 2px; }"
                  "QPushButton:hover { background: #8A8A8A; color: #eee; }");

    m_label = new QLabel(text, this);
    m_label->setFixedHeight(16);

    QPushButton *button = new QPushButton("x", this);
    button->setFixedHeight(16);
    connect(button, SIGNAL(clicked()), this, SLOT(deleteMe()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_label);
    layout->addWidget(button);
    setLayout(layout);
}

void TagWidget::deleteMe()
{
    delete this;
}

QMap<QString, int> TagWidget::all()
{
    QMap<QString, int> tags;

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.exec("SELECT id, name FROM tags");

    while (query.next())
    {
        tags.insert(query.value(1).toString(), query.value(0).toInt());
    }

    return tags;
}

int TagWidget::insertInDb(const QString& name)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery q(db);
    q.prepare("INSERT INTO tags(name) VALUES (:name)");
    q.bindValue(":name", name);
    !q.exec();

    return SqlUtils::lastInsertId();
}
