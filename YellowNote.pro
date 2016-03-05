QT += widgets
QT += sql
QT += network

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    notelabel.cpp \
    notedialog.cpp \
    note.cpp \
    notelistwidgetitem.cpp \
    o2/o2.cpp \
    o2/o2reply.cpp \
    o2/o2replyserver.cpp \
    o2/o2requestor.cpp \
    o2/o2settingsstore.cpp \
    o2/simplecrypt.cpp \
    oauth2.cpp \
    connexiondialog.cpp \
    sqlutils.cpp \
    hotkeythread.cpp \
    noteedit.cpp \
    notepanel.cpp \
    about.cpp \
    tag/tagcontainer.cpp \
    tag/tagedit.cpp \
    tag/tagwidget.cpp \
    tag/tag.cpp

HEADERS += \
    mainwindow.h \
    notedialog.h \
    note.h \
    notelistwidgetitem.h \
    o2/o2.h \
    o2/o2abstractstore.h \
    o2/o2globals.h \
    o2/o2reply.h \
    o2/o2replyserver.h \
    o2/o2requestor.h \
    o2/o2settingsstore.h \
    o2/simplecrypt.h \
    oauth2.h \
    connexiondialog.h \
    sqlutils.h \
    hotkeythread.h \
    noteedit.h \
    notepanel.h \
    about.h \
    tag/tagcontainer.h \
    tag/tagedit.h \
    tag/tagwidget.h \
    tag/tag.h

RC_FILE = app.rc

RESOURCES += \
    resources.qrc
