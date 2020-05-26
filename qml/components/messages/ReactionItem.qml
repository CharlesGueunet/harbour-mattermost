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
    property bool    isAddButton: false

    signal reactionClicked

    onReactionPathChanged: {
        if( reactionPath.indexOf("file://") == -1 )
        {
            if( !isAddButton )
                reactionRow.noImage = true;
        }
    }

    Rectangle {
        id: reactionRect
        anchors.fill: reactionRow
        property color rectColor : reactionItem.reactionIsMine ? Theme.highlightColor : Theme.secondaryColor
        color: Theme.rgba(rectColor , reactionItem.reactionIsMine ? 0.25 : 0.1)
        radius: Theme.paddingSmall
        border.width: 1
        border.color: rectColor
    }

    Row {
        id: reactionRow
        spacing: Theme.paddingSmall
        leftPadding: Theme.paddingSmall
        rightPadding: leftPadding
        layoutDirection: isAddButton ? Qt.RightToLeft : Qt.LeftToRight
        property bool noImage : reactionItem.reactionPath.length == 0

        Image {
            id: reactionImage
            source: reactionRow.noImage ? "image://theme/icon-s-warning" : Qt.resolvedUrl(reactionItem.reactionPath)
            width: context.reactionSize
            height: textCount.contentHeight
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Image.AlignHCenter
            verticalAlignment: Image.AlignVCenter
            visible: !reactionRow.noImage
            layer.enabled: isAddButton
            layer.effect: ShaderEffect {
                // grayscale effect
                property variant src: reactionImage
                property color highlight: btnAddSameReaction.pressed ? Theme.highlightColor : Theme.primaryColor
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
                            uniform lowp vec4 highlight;
                            uniform lowp float qt_Opacity;
                            void main() {
                                lowp vec4 tex = texture2D(src, coord);
                                gl_FragColor = vec4(vec3(dot(tex.rgb,
                                                    vec3(0.344, 0.5, 0.156))),
                                                         tex.a) * qt_Opacity * highlight;
                            }"
            }
        }

        Text {
            id: textCount
            text: isAddButton ? "+" : ((reactionRow.noImage?(reactionItem.reactionEmoji + " "):"") + String(reactionItem.reactionCount))
            font.pixelSize: context.reactionSize
            width: contentWidth
            color: (reactionItem.reactionIsMine || (isAddButton && btnAddSameReaction.pressed)) ? Theme.highlightColor : Theme.primaryColor
            anchors.verticalCenter: parent.verticalCenter
            font.bold: reactionItem.reactionIsMine || isAddButton
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
