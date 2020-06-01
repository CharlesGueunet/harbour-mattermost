TEMPLATE = subdirs
CONFIG += ordered

ADD_WEBSOCKET=""
static_websocket : {
    ADD_WEBSOCKET = libs/qtwebsockets/src/websockets
}
SUBDIRS += $$ADD_WEBSOCKET
SUBDIRS += harbour-mattermost-app.pro
