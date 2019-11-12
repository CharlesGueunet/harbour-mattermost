import QtQuick 2.5
import Sailfish.Silica 1.0
import ru.sashikknox 1.0
import "../pages"
import "../components"

ListItem {
    id: bgitem

    contentHeight: Theme.itemSizeMedium

    Column {
        anchors.fill: parent
        Label {
            id: teamname
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            font.pixelSize: Theme.fontSizeLarge
            text: role_display_name
        }
        Row {
            spacing: Theme.paddingMedium
            Image {
                id: message_icon
                source: "image://theme/icon-s-message"
                width: Theme.iconSizeSmall
                height: Theme.iconSizeSmall
            }
            Label {
                id: message_count
                text: role_unread_message
            }
            Image {
                id: mention_icon
                source: "image://theme/icon-s-warning"
                width: Theme.iconSizeSmall
                height: Theme.iconSizeSmall
            }
            Label {
                id: mention_count
                text: role_unread_mention
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
                                   context:      teamsPage.context
                               } )
        pageStack.navigateForward(PageStackAction.Animated);
    }
}
