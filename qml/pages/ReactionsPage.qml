import QtQuick 2.0
import Sailfish.Silica 1.0
import ru.sashikknox 1.0

Page {
    id: reactionsPage
    clip: true

    VerticalScrollDecorator {
        flickable: gridView
    }

    anchors.leftMargin: Theme.paddingLarge
    anchors.rightMargin: Theme.paddingLarge

    property EmojiModel emojiModel: EmojiModel {}

    SilicaListView {
        id: gridView

        highlightMoveDuration: 300

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: panel.top
        }
        clip: true
        anchors.bottomMargin: panel.margin

        header: PageHeader {
            title: qsTr("Reactions")
            leftMargin: Theme.paddingLarge
            rightMargin: Theme.paddingLarge
        }
        spacing: Theme.paddingMedium
        model:emojiModel.categories

        footer: Component {
            BackgroundItem {
                anchors.left: parent.left
                anchors.right: parent.right
                height: Theme.paddingMedium
                TouchBlocker {
                    anchors.fill: parent
                }
            }
        }

        delegate: Column {

            id: categoryColumn
            width: parent.width
            height: sectionHeader.height + emojiGrid.height + spacing
            spacing: Theme.paddingMedium
//            anchors.leftMargin: Theme.paddingMedium
//            anchors.rightMargin: Theme.paddingMedium

            property string categoryName: modelData
            SectionHeader {
                id: sectionHeader
                text: categoryColumn.categoryName
                height: Theme.itemSizeSmall
                anchors.left: parent.left
                anchors.right: parent.right
                leftPadding: Theme.paddingLarge
                rightPadding: Theme.paddingLarge
            }

            Grid {
                id: emojiGrid
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: Theme.paddingMedium
                anchors.rightMargin: Theme.paddingMedium
                property real emojiItemSize: Theme.fontSizeHuge
                columnSpacing: 0
                rowSpacing: 0

                columns: (parent.width - anchors.leftMargin * 2) / emojiGrid.emojiItemSize


                property real emojiItemWidth: emojiGrid.emojiItemSize + ((parent.width - anchors.leftMargin * 2) - columns * emojiGrid.emojiItemSize)/columns

                Repeater {
                    model: EmojiCategoryProxy {
                        sourceModel: emojiModel
                        category: categoryColumn.categoryName;
                    }

                    delegate: BackgroundItem {
                        width: emojiGrid.emojiItemWidth
                        height: emojiGrid.emojiItemSize

                        Image {
                            source: role_image
                            anchors.fill: parent
                            anchors.margins: Theme.paddingSmall
                            fillMode: Image.PreserveAspectFit
                            cache: false
                            asynchronous: true
                            Behavior on opacity {
                                NumberAnimation { duration: 200 }
                            }
                            visible: status == Image.Ready
                            opacity: visible ? 1 : 0
                        }

                        onClicked: {
                            console.log("Emoji choosed: " + role_name + " from category " + role_category)
                        }
                    }
                }
            }
        }
    }

    SilicaFlickable {
        id: panel

        width: parent.width
        height: Theme.itemSizeMedium //+ Theme.paddingSmall

        contentWidth: buttonsRow.width

        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.HorizontalFlick

        anchors.leftMargin: Theme.paddingLarge
        anchors.rightMargin: Theme.paddingLarge
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        property real buttonSize : Theme.iconSizeMedium

        Row {
            id: buttonsRow
            spacing: Theme.paddingMedium
            anchors.verticalCenter: parent.verticalCenter

            Repeater {
                model: emojiModel.categories

                IconButton {
                    id: iconButton
                    icon.source: emojiModel.categoryIcon(modelData);
                    width: panel.buttonSize
                    height: panel.buttonSize
                    icon.width: panel.buttonSize - Theme.paddingSmall * 2
                    icon.height: panel.buttonSize - Theme.paddingSmall * 2
                    icon.color: "white"
                    icon.layer.enabled: false
                    icon.layer.effect: ShaderEffect {
//                        width: iconButton.icon.width
//                        height: iconButton.icon.height
                        property variant src: parent.icon
                        vertexShader: "
                                    uniform highp mat4 qt_Matrix;
                                    attribute highp vec4 qt_Vertex;
                                    attribute highp vec2 qt_MultiTexCoord0;
                                    varying highp vec2 coord;
                                    void main() {
                                        coord = qt_MultiTexCoord0;
                                        gl_Position = qt_Matrix * qt_Vertex;
                                    }"
                        fragmentShader: "
                                    varying highp vec2 coord;
                                    uniform sampler2D src;
                                    uniform lowp float qt_Opacity;
                                    void main() {
                                        lowp vec4 tex = texture2D(src, coord);
                                        gl_FragColor = vec4(vec3(dot(tex.rgb,
                                                            vec3(0.344, 0.5, 0.156))),
                                                                 tex.a) * qt_Opacity;
                                    }"
                    }
                    onClicked: {
                        gridView.positionViewAtIndex( index, ListView.Beginning  )
                    }
                }
            }
        }
    }
}
