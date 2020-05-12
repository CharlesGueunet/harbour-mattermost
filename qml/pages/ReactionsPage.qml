import QtQuick 2.0
import Sailfish.Silica 1.0
import ru.sashikknox 1.0

Page {
    id: reactionsPage

    VerticalScrollDecorator {
        flickable: gridView
    }

    anchors.leftMargin: Theme.paddingLarge
    anchors.rightMargin: Theme.paddingLarge

    SilicaGridView {
        id: gridView
        anchors.fill: parent
        header: PageHeader {
            title: "Reactions"
            leftMargin: Theme.paddingLarge
            rightMargin: Theme.paddingLarge
        }

//        anchors.margins: Theme.paddingLarge
        cellWidth:  Theme.fontSizeHuge
        cellHeight: cellWidth
        model: EmojiModel {
            category: "Other"
        }

        delegate: BackgroundItem {
            width: gridView.cellWidth
            height: gridView.cellHeight

            Image {
                source: role_image
                anchors.fill: parent
                anchors.margins: Theme.paddingSmall
            }

            onClicked: {
                console.log("Emoji choosed: " + role_name + " from category " + role_category)
            }
        }
    }
}
