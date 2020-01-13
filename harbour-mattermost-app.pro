# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-mattermost

VERSION = 0.1.3-16

DEFINES += MATTERMOSTQT_VERSION=\\\"$${VERSION}\\\"
DEFINES += EMOJI_PATH=\\\"/usr/share/$$TARGET/emoji\\\"

dbus.files = dbus/sashikknox.mattermost.service
dbus.path = /usr/share/dbus-1/services/
#INSTALLS += dbus


CONFIG += sailfishapp warnoff
CONFIG += qt
QT += gui qml quick network dbus svg
PKGCONFIG += nemonotifications-qt5 sailfishsilica
#PKGCONFIG += libgcrypt

LIBS += -Llibs -lqt5websockets
INCLUDEPATH += src/
INCLUDEPATH += libs/qtwebsockets/include
INCLUDEPATH += libs/qtwebsockets/src/websockets

include(libs/discount.pri);

emoji.files = $$PWD/resources/emoji/emoji.json
emoji.path = /usr/share/$$TARGET/emoji/
emoji_svg.files = $$PWD/resources/emoji/svg
emoji_svg.path = /usr/share/$$TARGET/emoji/
INSTALLS += emoji emoji_svg

debug: DEFINES += _DEBUG
!debug: DEFINES += _RELEASE
!debug: DEFINES -= _DEBUG

SOURCES += src/harbour-mattermost.cpp \
    src/TeamsModel.cpp \
    src/MattermostQt.cpp \
    src/ChannelsModel.cpp \
    src/MessagesModel.cpp \
    src/SailNotify.cpp \
    src/AccountsModel.cpp \
    src/SettingsContainer.cpp \
    src/MarkdownParser.cpp \
    src/DiscountMDParser.cpp \
    src/AttachedFilesModel.cpp \
    src/cpphash.cpp

DISTFILES += \
    resources/emoji/* \
    rpm/harbour-mattermost.changes.run.in \
    rpm/harbour-mattermost.spec \
    translations/*.ts \
    harbour-mattermost.desktop \
    server.pri \
    translations/harbour-mattermost-ru.ts \
    rpm/harbour-mattermost.yaml \
    dbus/sashikknox.mattermost.service \
    CHANGELOG \
    CHANGELOG \
    LICENSE \
    rpm/harbour-mattermost.changes

SAILFISHAPP_ICONS = 86x86 108x108 128x128

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n


# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += \
    translations/harbour-mattermost.ts \
    translations/harbour-mattermost-ru.ts \
    translations/harbour-mattermost-de.ts

HEADERS += \
    src/MattermostQt_gobal.h \
    src/TeamsModel.h \
    src/MattermostQt.h \
    src/ChannelsModel.h \
    src/MessagesModel.h \
    src/SailNotify.h \
    src/AccountsModel.h \
    src/SettingsContainer.h \
    src/MarkdownParser.h \
    src/DiscountMDParser.h \
    src/AttachedFilesModel.h \
    src/c_cpphash.h \
    src/cpphash.h

RESOURCES += \
    resources.qrc
