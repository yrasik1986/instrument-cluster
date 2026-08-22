import QtQuick

Item {
    id: root

    property string text: ""
    property bool active: false
    property color color: "white"

    width: 100
    height: 30

    opacity: active
             ? (blinkTimer.blinkState ? 1.0 : 0.25)
             : 0.25

    Timer {
        id: blinkTimer

        property bool blinkState: true

        interval: 500
        repeat: true
        running: root.active

        onTriggered: {
            blinkState = !blinkState
        }

        onRunningChanged: {
            if (!running)
                blinkState = true
        }
    }

    Text {
        anchors.centerIn: parent

        text: root.text

        color: root.color

        font.pixelSize: 18
        font.bold: true
    }
}