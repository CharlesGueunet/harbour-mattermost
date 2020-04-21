import QtQuick 2.2
import QtQuick.Controls 1.4
//import ru.sashikknox 1.0;

Item {
//    property QuickObject object: QuickObject {}
    
    TextArea {
        id: textObject
        textFormat: Text.RichText
        objectName: "textObject"
        text: "object.text"
        anchors.fill: parent
        anchors.margins: 0
        font.pixelSize: 12
    }
}
