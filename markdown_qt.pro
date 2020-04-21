TEMPLATE = app
CONFIG += qt
QT += core widgets gui quick qml quickwidgets

INCLUDEPATH += $$PWD/tools/markdown_qt
INCLUDEPATH += $$PWD/src

DEFINES += EMOJI_PATH=\\\"$$PWD/resources/emoji\\\"
DEFINES += DESKTOP_APP

TARGET = markdown_qt

QMAKE_CFLAGS += -fno-permissive 

SOURCES += \
    tools/markdown_qt/main.cpp \
    tools/markdown_qt/mdMain.cpp \
    src/cpphash.cpp

#LIBS += -L$$PWD/thirdparty/discount-2.2.4 -lmarkdown

FORMS += \
    tools/markdown_qt/mdMain.ui

HEADERS += \
    tools/markdown_qt/mdMain.h \
    src/c_cpphash.h

include(libs/discount.pri)

#RESOURCES += \
#    res.qrc

DISTFILES += \
    .gitignore \
    emoji.json \
    emoji_pretty.json \
    emoji_utf8.rb \
    gistfile1.md

RESOURCES += \
    tools/markdown_qt/res.qrc
