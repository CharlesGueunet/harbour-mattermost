import QtQuick 2.5
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
import ru.sashikknox 1.0
import "../pages"
import "../components"

ListItem {
    id: bgitem

    anchors.leftMargin: Theme.paddingLarge
    anchors.rightMargin: Theme.paddingLarge
    contentHeight: Theme.itemSizeLarge
    property int unread_count : role_unread_message + role_unread_mention

    property string imagePath : (role_team_icon.length !== 0) ? role_team_icon : Qt.resolvedUrl("qrc:/resources/logo_rect_white.png") + String("?") + Theme.rgba(Theme.primaryColor,Theme.opacityLow)
    Image {
        id: teamIcon
        source: bgitem.imagePath
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingLarge
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.itemSizeMedium
        height: width

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: maskRect
        }

        Rectangle {
            id: maskRect
            anchors.fill: teamIcon
            radius: Theme.paddingMedium
            visible: false
        }
    }


    Column {
        spacing: Theme.paddingSmall
        anchors.left: teamIcon.right
        anchors.right: parent.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        Label {
            id: teamname
            anchors.leftMargin: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeLarge
            text: role_display_name
        }

        Rectangle {
            id: unreadLabelRect
            color: unread_count > 0 ? Theme.rgba("white",0.8) : Theme.rgba(0,0,0,0)
            width: Math.max(context.avatarSize * 0.5, labelUnreadCount.width  + labelUnreadCount.anchors.leftMargin )
            height: labelUnreadCount.height
            radius: height * 0.5

            Row {
                id: labelUnreadCount
                spacing: Theme.paddingMedium
                anchors.leftMargin: Theme.paddingMedium
                anchors.rightMargin: Theme.paddingMedium
                Image {
                    id: message_icon
                    source: "image://theme/icon-s-message?" + ( unread_count > 0 ? "black" : Theme.primaryColor )
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    id: message_count
                    text: role_unread_message
                    color: unread_count > 0 ? "black" : Theme.primaryColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                Image {
                    id: mention_icon
                    source: "image://theme/icon-s-warning?" + ( unread_count > 0 ? "black" : Theme.primaryColor )
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    id: mention_count
                    text: role_unread_mention
                    color: unread_count > 0 ? "black" : Theme.primaryColor
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }



    onClicked: {
        pageStack.pushAttached( Qt.resolvedUrl("../pages/ChannelsPage.qml"),
                               {
                                   teamid:       role_team_id,
                                   server_index: role_server_index,
                                   team_index:   role_team_index,
                                   team_label:   teamname.text,
                                   context:      teamsPage.context,
                                   teamIcon:     imagePath
                               } )
        pageStack.navigateForward(PageStackAction.Animated);
    }
}
