import QtQuick 2.5
import QtMultimedia 5.6
import QtSensors 5.1
import Sailfish.Silica 1.0

FullscreenContentPage {
    id: cameraPicker
    allowedOrientations: Orientation.All

    property string capturedImagePath: ""

    signal uploadImage

    Rectangle {
        id: background
        color: Qt.rgba(0,0,0,1) // black
    }

    Item {
        anchors.fill: parent

        Camera {
            id: camera
            position: Camera.BackFace //Camera.FrontFace

            imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash

            exposure {
                exposureCompensation: -1.0
                exposureMode: Camera.ExposurePortrait
            }

            flash.mode: Camera.FlashRedEyeReduction

            focus {
                focusMode: Camera.FocusHyperfocal
                focusPointMode: Camera.FocusPointCustom
            }

            imageCapture {

                onImageCaptured: {
                    photoPreview.source = preview  // Show the preview in an Image
                    photoPreview.visible = true
                    camera.stop();
                    console.log("Camera captured image " + preview)
                }

                onImageSaved: {
                    photoPreview.source = path  // Show the preview in an Image
                    photoPreview.visible = true
                    camera.stop();
                    console.log("Camera saved image " + path)
                }

                onCaptureFailed: {
                    console.log("Capture failed: " + message)
                }

                onCapturedImagePathChanged: {
                    cameraPicker.capturedImagePath = camera.imageCapture.capturedImagePath
                    console.log("Capture path changed: " + camera.imageCapture.capturedImagePath)
                }
            }

            Component.onCompleted: {
                start();
                unlock();
                console.log("Qt.Key_CameraFocus" + String(Qt.Key_CameraFocus))
                console.log("Qt.Key_Camera" + String(Qt.Key_Camera))
            }

            metaData.orientation: orientationSensor.imageRotation
        }

        VideoOutput {
            id: videoOutput
            source: camera
            anchors.centerIn: parent
            focus : visible // to receive focus and capture key events when visible
            rotation: orientationSensor.rotationAngle

            Component.onCompleted: {
                width = parent.width
                height = parent.height
                console.log( "Current resolutions " + String(camera.viewfinder.resolution) )
            }
        }

        OrientationSensor {
            id: orientationSensor
            active: true
            property int rotationAngle: cameraPicker.orientation ? getOrientation(cameraPicker.orientation) : 0
            property int imageRotation: cameraPicker.orientation ? getImageOrientation(cameraPicker.orientation) : 0
            property int orientation: reading.orientation ? reading.orientation : 0

            function getOrientation(value) {
                switch (value) {
                case Orientation.Portrait:
                    console.log("Portrait")
                    return 0
                case Orientation.LandscapeInverted:
                    console.log("LandscapeInverted")
                    return 90
                case Orientation.PortraitInverted:
                    console.log("PortraitInverted")
                    return 180
                default:
                    console.log("Landscape")
                    return 270
                }
            }

            function getImageOrientation(value) {
                switch (value) {
                case Orientation.Portrait:
                    console.log("Image Portrait")
                    if( camera.position == Camera.BackFace )
                        return 270
                    else
                        return 90
                case Orientation.LandscapeInverted:
                    console.log("Image LandscapeInverted")
                    return 180
                case Orientation.PortraitInverted:
                    console.log("Image PortraitInverted")
                    return 90
                default:
                    console.log("Image Landscape")
                    return 0
                }
            }

            onRotationAngleChanged: {
                console.log("Orientation changed " + rotationAngle)
            }
        }

        Image {
            id: photoPreview
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            autoTransform: true
            visible: false

            Item {
                id: buttons

                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                    margins: Theme.paddingLarge
                }

                width: height * 2 + Theme.paddingLarge
                height: Theme.iconSizeLarge

                IconButton {
                    id: acceptImage
                    icon.source: "image://theme/icon-m-cloud-upload"
                    anchors.right: parent.right
                    anchors.leftMargin: Theme.paddingLarge * 0.5
                    icon.width: buttons.height
                    icon.height: buttons.height
                    width: buttons.height
                    height: width
                    onClicked: {
                        console.log("Upload image: " + cameraPicker.capturedImagePath )
                        uploadImage()
                        pageStack.pop()
                    }
                }

                IconButton {
                    id: deleteImage
                    icon.source: "image://theme/icon-m-delete"
                    anchors.left: parent.left
                    anchors.rightMargin: Theme.paddingLarge * 0.5
                    icon.width: buttons.height
                    icon.height: buttons.height
                    width: buttons.height
                    height: width
                    onClicked: {
                        console.log("Delete image: " + cameraPicker.capturedImagePath )
                        camera.start()
                        camera.unlock()
                        photoPreview.visible = false
                    }
                }
            }
        }
    }

    MouseArea {
        id: focusTouchArea
        anchors.fill: parent
        visible: buttonsPanel.visible

        onClicked:  {
            camera.focus.focusMode = CameraFocus.FocusPointCustom
            camera.focus.focusPointMode = CameraFocus.FocusPointCustom
            camera.focus.customFocusPoint = Qt.point(mouseX,mouseY)
            camera.searchAndLock();

            focusRect.visible = true
            focusRect.x = mouseX - focusRect.radius
            focusRect.y = mouseY - focusRect.radius
        }

        Rectangle {
            id: focusRect
            visible: false
            width: Theme.iconSizeLarge * 2
            height: width
            radius: width * 0.5
            color: Qt.rgba(0,0,0,0)
            border.color: Theme.highlightColor
            border.width: Theme.paddingSmall * 0.5
        }
    }

    IconButton {
        id: closeCameraPicker

        icon.source: "image://theme/icon-m-close"
        width: Theme.iconSizeMedium + Theme.paddingSmall
        height: width

        anchors {
            margins: Theme.paddingLarge
            top: parent.top
            right: parent.right
        }

        onClicked: {
            pageStack.pop()
        }
    }

    Item {
        id: buttonsPanel
        visible: !photoPreview.visible

        property real panelSize: Theme.itemSizeHuge

        height: Theme.itemSizeHuge

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        Rectangle {
            id: changeCamera
            width: Theme.iconSizeLarge
            height: width
            radius: width * 0.5
            color: Theme.rgba(Theme.highlightBackgroundColor, 0.15)

            anchors {
                margins: Theme.paddingLarge
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            IconButton {
                anchors.fill: parent
//                anchors.margins: Theme.paddingSmall

                icon.source: "image://theme/icon-m-sync" //+ (pressed ? Theme.highlightColor : Theme.primaryColor)

                onClicked: {
                    camera.stop()
                    if( camera.position == Camera.BackFace ) {
                        camera.position = Camera.FrontFace
                    } else {
                        camera.position = Camera.BackFace
                    }
                    camera.start()
                    camera.unlock()
                }
            }
        }

        Rectangle {
            id: shotButton
            border.width: Theme.iconSizeSmall * 0.075
            border.color: "white"

            color: Qt.rgba(1.0,1.0,1.0,0.3)
            width: Theme.iconSizeExtraLarge + Theme.paddingMedium
            height: width
            radius: width * 0.5

            anchors {
                centerIn: parent
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    camera.imageCapture.capture();
                }

                onPressed: {
                    shotButton.color = Qt.rgba(1.0,1.0,1.0,0.7)
                }

                onReleased: {
                    shotButton.color = Qt.rgba(1.0,1.0,1.0,0.3)
                }
            }
        } // Rectangle shotButton
    }// Item buttonsPanel

    Keys.enabled: true
    Keys.priority: Keys.BeforeItem
    Keys.onPressed: {
        if( event.key == Qt.Key_CameraFocus )
        {
            camera.focus.focusMode = Camera.FocusHyperfocal//CameraFocus.FocusManual
            camera.focus.focusPointMode = CameraFocus.FocusPointCenter
            //camera.focus.customFocusPoint = Qt.point(parent.width*0.5,parent.height * 0.5)
            camera.searchAndLock();
//            var focuses = camera.focus.focusZones
//            for(var i = 0; i < focuses.length; i++ )
//            {
//                if ( viewfinder.status === Camera.FocusAreaUnused )
//                {
//                    camera.focus.focusMode = CameraFocus.FocusManual
//                    camera.focus.focusPointMode = CameraFocus.FocusPointCenter
//                    //camera.focus.customFocusPoint = Qt.point(parent.width*0.5,parent.height * 0.5)
//                    camera.searchAndLock();
//                    console.log("Focusing")
//                    break;
//                }
//                else if ( viewfinder.status === Camera.FocusAreaSelected )
//                {
//                    console.log("Focusing in process")
//                    break;
//                }
//            }
//            console.log("CameraFocus")
        }
        else
            console.log("Key is " + event.key)
        //            if( event.key )
        //            console.log(event.key)
    }

    states: [
        State {
            name: "portrait"
            when: orientation == Orientation.Portrait
            AnchorChanges {
                target: buttonsPanel
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: parent.left
                    top: undefined
                }
            }

            AnchorChanges {
                target: changeCamera
                anchors {
                    right: parent.right
                    top: undefined
                    bottom: undefined
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: undefined
                }
            }

            AnchorChanges {
                target: closeCameraPicker
                anchors {
                    top: parent.top
                    left: undefined
                    right: parent.right
                    bottom: undefined
                }
            }

            PropertyChanges {
                target: buttonsPanel
                height: panelSize
            }
        },
        State {
            name: "landscape"
            when: orientation == Orientation.Landscape

            AnchorChanges {
                target: buttonsPanel
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: undefined
                    top: parent.top
                }
            }

            AnchorChanges {
                target: changeCamera
                anchors {
                    right: undefined
                    top: parent.top
                    bottom: undefined
                    verticalCenter: undefined
                    horizontalCenter: parent.horizontalCenter
                }
            }

            AnchorChanges {
                target: closeCameraPicker
                anchors {
                    top: parent.top
                    left: parent.left
                    right: undefined
                    bottom: undefined
                }
            }

            PropertyChanges {
                target: buttonsPanel
                width: panelSize
            }
        },
        State {
            name: "landscape_inverted"
            when: orientation == Orientation.LandscapeInverted
            AnchorChanges {
                target: buttonsPanel
                anchors {
                    bottom: parent.bottom
                    right: undefined
                    left: parent.left
                    top: parent.top
                }
            }

            AnchorChanges {
                target: changeCamera
                anchors {
                    right: undefined
                    top: undefined
                    bottom: parent.bottom
                    verticalCenter: undefined
                    horizontalCenter: parent.horizontalCenter
                }
            }

            AnchorChanges {
                target: closeCameraPicker
                anchors {
                    top: undefined
                    left: undefined
                    right: parent.right
                    bottom: parent.bottom
                }
            }

            PropertyChanges {
                target: buttonsPanel
                width: panelSize
            }
        }
    ]// states
}
