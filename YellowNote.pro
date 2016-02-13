QT += widgets
QT += sql
QT += webkit

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
    sqlutils.cpp

HEADERS += \
    mainwindow.h \
    notelabel.h \
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
    sqlutils.h

RC_FILE = app.rc

RESOURCES += \
    resources.qrc
