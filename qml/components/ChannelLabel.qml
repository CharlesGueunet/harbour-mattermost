import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
import ru.sashikknox 1.0
import "../model"
import "../components"

Item {
    id: channellabel

    property string _display_name
    property string _header
    property string _purpose
    property int  _index
    property int _type
    property int channelType
    property string directChannelImage
    property int directChannelUserStatus: MattermostQt.UserNoStatus
    property int _channel_unread: 0 // summary count of unread messages and mention
    property int _users_typing: 0
    property real user_typeing_opacity : 0
    property Context context

    height: loader.itemHeight

    on_Users_typingChanged: {
        if( _users_typing > 0 )
            user_typeing_opacity = 1
        else
            user_typeing_opacity = 0
    }

    Component {
        id: simpleIcon
        Icon {
            source:
                switch(channelType) {
                case MattermostQt.ChannelPrivate:
                    "image://theme/icon-m-device-lock"
                    break
                default: // MattermostQt.HeaderPublic:
                    "image://theme/icon-m-chat"
                }
            width: Theme.iconSizeMedium
            height: Theme.iconSizeMedium
        }
    }

    Component {
        id: avatarIcon
        UserAvatar {
            context: channellabel.context
            userStatus: directChannelUserStatus
            imagePath: directChannelImage
        }
    }

    Component {
        id: channel_line
        Row {
            spacing: Theme.paddingMedium
            Loader {
                id: channel_icon
                sourceComponent:
                    switch(channelType) {
                    case MattermostQt.ChannelDirect:
                        avatarIcon
                        break
                    default:
                        simpleIcon
                    }
            }

            Column {
                Label {
                    id: labelname
                    text: _display_name
                    height: contentHeight
                    onHeightChanged: itemHeight = height
                }

                Label {
                    id: usersTyping
                    visible: opacity > 0
                    opacity: user_typeing_opacity

                    Behavior on opacity {
                        NumberAnimation { duration: 200 }
                    }
                    font.pixelSize: Theme.fontSizeTiny
                    // TODO make beautiful string with user names, or its count if it too much
                    text: qsTr("Typing") + ": " + _users_typing
                }
            }

            Rectangle {
                id: unreadLabelRect
                visible: _channel_unread > 0
                color: Theme.rgba(Theme.primaryColor,0.8)
                width: Math.max(context.avatarSize * 0.5, labelUnreadCount.contentWidth  + labelUnreadCount.anchors.margins + labelUnreadCount.contentHeight )
                height: labelUnreadCount.contentHeight + labelUnreadCount.anchors.margins
                radius: height * 0.5

//                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: Theme.paddingMedium

                Label {
                    id: labelUnreadCount
                    anchors.centerIn: parent
//                    color: Theme.highlightColor
                    color: "black"
                    text: String(_channel_unread)
                    font.pixelSize: Theme.fontSizeSmall
                }
            }
        }
    }

    Component {
        id: channel_goup
        SectionHeader {
            TouchBlocker {
                     anchors.fill: parent
            }
            text: switch(_type) {
                  case ChannelsModel.HeaderPublic:
                      qsTr("Public channes")
                      break;
                  case ChannelsModel.HeaderPrivate:
                      qsTr("Private channes")
                      break;
                  case ChannelsModel.HeaderDirect:
                      qsTr("Direct channes")
                      break;
                 }
            onHeightChanged: itemHeight = height
        }
    }

    Loader {
        id: loader
        property real itemHeight
        anchors {left: parent.left; right: parent.right; }
        anchors {
            topMargin: _type == ChannelsModel.Channel ? Theme.horizontalPageMargin : Theme.paddingSmall
            bottomMargin: loader.topMargin
        }

        sourceComponent:
            switch(_type)
            {
            case ChannelsModel.HeaderPublic:
            case ChannelsModel.HeaderPrivate:
            case ChannelsModel.HeaderDirect:
                channel_goup
                break;
            default:
                channel_line
            }
    }
}
