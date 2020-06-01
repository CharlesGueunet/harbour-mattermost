import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQml.Models 2.2
import "../model"

Page {
    id: aboutpage
    property Context context

    onContextChanged: {
        versionlabel.text = qsTr("Version: ") + context.mattermost.getVersion()
    }

    SilicaListView {
        id: flickable
        anchors.fill: parent
        VerticalScrollDecorator {}

        model: ObjectModel {
            id: elements
            Item {
                id: topItem
                height: Theme.itemSizeSmall
            }

            Image {
                id: appicon
                source: "qrc:/resources/mattermost_icon.svg"
                width: 0.3*((aboutpage.width>aboutpage.height)?aboutpage.height:aboutpage.width)
                height: width
                anchors.horizontalCenter: parent.horizontalCenter
                anchors {
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                    topMargin: Theme.paddingMedium
                    bottomMargin: Theme.paddingMedium
                }
            }

            Label {
                id: versionlabel
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text : qsTr("Version: ") + (context !== null)?context.mattermost.getVersion():"0.0.0"
                anchors {
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                    topMargin: Theme.paddingMedium
                    bottomMargin: Theme.paddingLarge
                    //                horizontalCenter: parent.horizontalCenter
                    left: parent.left
                    right: parent.right
                }
            }

            Item { height: Theme.paddingLarge }

            LinkedLabel {
                id: sourceslink
                text: qsTr("Sources: ") + "<a href=\"https://gitlab.com/sashikknox/harbour-mattermost\">GitLab</a><br>"
                      + qsTr("Using Emoji from") + " <a href=\"https://twemoji.twitter.com/\">Twemoji</a>"
                width: parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignLeft
                anchors {
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                    topMargin: Theme.paddingMedium
                    bottomMargin: Theme.paddingMedium
                    //                horizontalCenter: parent.horizontalCenter
                    left: parent.left
                    right: parent.right
                }
            }

            Item { height: Theme.paddingLarge }

            LinkedLabel {
                id: donatelink
                text: qsTr("If you want to donate, you can do that by:")
                      + " <br> <a href=\"http://yasobe.ru/na/sashikknox\">"
                      + qsTr("Yandex Money") + "</a>"
                width: parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignLeft
                anchors {
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                    topMargin: Theme.paddingMedium
                    bottomMargin: Theme.paddingMedium
                    //                horizontalCenter: parent.horizontalCenter
                    left: parent.left
                    right: parent.right
                }
            }

            Item { height: Theme.paddingLarge }

            LinkedLabel {
                id: aboutlabel
                text: qsTr("This is unofficial client for")
                      + " <a href=\"https://www.mattermost.org/licensing/\">Mattermost</a> "
                      + qsTr("server.") + " "
                      + qsTr("Thanks to")
                      + " <a href=\"https://t.me/sailfishos/\">"
                      + qsTr("Russian SailfishOS Community channel")
                      + "</a> " + qsTr("in Telegram for their help.")
                      + "<br><br>" + qsTr("Emoji categories icon design - ") + "<a href=\"https://t.me/timxez\">@timxez</a>"
                      + qsTr(" from Junnxy studio (links below):")
                      + "<br><a href=\"https://instagram.com/junnxystudio\">Junnxy on Instagram</a>"
                      + "<br><a href=\"https://www.behance.net/timxez\">Timxez on Behance</a>"
                      + "<br><br>" + qsTr("And to users:")
                      + "<br>< a href=\"https://gitlab.com/alexus230\">Alexey Kovalev</a> "
                      + "<br>< a href=\"https://gitlab.com/dashinfantry\">dashinfantry</a>"
                      + "<br>< a href=\"https://gitlab.com/SlavaMonich\">Slava Monich</a>"

                width: parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignLeft
                anchors {
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                    topMargin: Theme.paddingMedium
                    bottomMargin: Theme.paddingMedium
                    //                horizontalCenter: parent.horizontalCenter
                    left: parent.left
                    right: parent.right
                }
            }

            Item {
                id: bottomItem
                height: Theme.itemSizeSmall
            }
        }// ObjectModel
    }// SilicaListView
}
