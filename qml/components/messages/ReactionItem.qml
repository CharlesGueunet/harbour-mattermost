import QtQuick 2.6
import Sailfish.Silica 1.0
import ru.sashikknox 1.0
import "../../model"
import ".."

Item {
    id: reactionItem
    width: reactionRow.width
    height: reactionRow.height

    property Context context
    property string  reactionPath : ""
    property int     reactionCount: 0
    property bool    reactionIsMine: false
    property string  reactionEmoji: ""
//    property color   textColor: reactionIsMine ? Theme.secondaryColor : Theme.primaryColor

    signal reactionClicked

    onReactionPathChanged: {
        if( reactionPath.indexOf("file://") == -1 )
        {
            reactionRow.noImage = true;
        }
    }

    Rectangle {
        id: reactionRect
        anchors.fill: reactionRow
        color: Theme.rgba(reactionItem.reactionIsMine ? Theme.primaryColor : Theme.secondaryColor , reactionItem.reactionIsMine ? 0.25 : 0.1)
        radius: Theme.paddingSmall
        border.width: 1
        border.color: Theme.primaryColor
    }

    Row {
        id: reactionRow
        spacing: Theme.paddingSmall
        leftPadding: Theme.paddingSmall
        rightPadding: Theme.paddingSmall
        property bool noImage : reactionItem.reactionPath.length == 0

        Image {
            width: context.reactionSize
            height: context.reactionSize
            source: reactionRow.noImage ? "image://theme/icon-s-warning" : reactionItem.reactionPath
            anchors.verticalCenter: parent.verticalCenter
            visible: !reactionRow.noImage
        }

        Text {
            text: (reactionRow.noImage?(reactionItem.reactionEmoji + " "):"") + String(reactionItem.reactionCount)
            font.pixelSize: context.reactionSize
            width: contentWidth
            color: reactionItem.reactionIsMine ? Theme.highlightColor : Theme.primaryColor
            anchors.verticalCenter: parent.verticalCenter
            font.bold: reactionItem.reactionIsMine
        }
    }

    MouseArea {
        id: btnAddSameReaction
        anchors.fill: reactionRect

        onClicked: {
            reactionItem.reactionClicked()
        }
    }
}
