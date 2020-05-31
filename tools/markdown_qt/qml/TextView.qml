import QtQuick 2.2
import QtQuick.Controls 1.4
//import ru.sashikknox 1.0;

Item {

    TextArea {
        id: textObject
        textFormat: Text.RichText
        objectName: "textObject"
        text: "Hello World!"
        anchors.margins: 0
        visible: true
        anchors.fill: parent
        font.pixelSize: 12
        width: parent.width
    }

}
