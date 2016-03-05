#include "tagcontainer.h"
#include "tag.h"
#include "tagwidget.h"
#include <QLabel>

TagContainer::TagContainer(Note *note, QWidget *parent) : QWidget(parent)
{
    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/note/tag"));

    m_container = new QHBoxLayout();

    m_tagEdit = new TagEdit(m_container, note, this);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(icon);
    mainLayout->addLayout(m_container);
    mainLayout->addWidget(m_tagEdit);
    setLayout(mainLayout);

    m_note = note;
    update();
}

void TagContainer::setNote(Note *note)
{
    m_note = note;
    m_tagEdit->setNote(m_note);
    update();
}

void TagContainer::update()
{
    if (m_note)
    {
        QList<Tag*> tags = Tag::findByNoteId(m_note->id());
        if ( ! tags.empty())
        {
            clearContainer();
            for (int i = 0; i < tags.size(); ++i)
            {
                TagWidget *tagWidget = new TagWidget(tags.at(i)->name());
                m_container->addWidget(tagWidget);
            }
        }
    }
}

void TagContainer::clearContainer()
{
    QLayoutItem* item;
    while ((item = m_container->takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }
}
