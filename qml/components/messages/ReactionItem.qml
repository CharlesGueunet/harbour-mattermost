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
    property color   textColor: Theme.primaryColor

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
        color: Theme.rgba(Theme.primaryColor,0.1)
        radius: Theme.paddingSmall
        border.width: 1
        border.color: Theme.primaryColor
    }

    Row {
        id: reactionRow
        spacing: Theme.paddingSmall
        leftPadding: Theme.paddingSmall
        rightPadding: Theme.paddingSmall
        property bool noImage : false



        Image {
            width: context.reactionSize
            height: context.reactionSize
            source: reactionRow.noImage ? "image://theme/icon-s-warning" : reactionItem.reactionPath
            anchors.verticalCenter: parent.verticalCenter
            visible: !reactionRow.noImage
        }

        Text {
            text: (reactionRow.noImage?(reactionItem.reactionPath + " "):"") + String(reactionItem.reactionCount)
            font.pixelSize: context.reactionSize
            width: contentWidth
            color: reactionItem.textColor
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        id: btnAddSameReaction
        anchors.fill: reactionRect

        onClicked: {
            console.log("Add reaction[" + String(index) + "] to post id(" + role_post_id + ")" )
            reactionItem.reactionClicked(index)
        }
    }
}
