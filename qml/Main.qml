import QtQuick
import QtQuick.Window

Window {
    id: root

    width: 1280
    height: 480

    minimumWidth: 1280
    minimumHeight: 480

    visible: true

    title: "Vehicle Instrument Cluster"

    color: "#090b0f"

    Dashboard {
        anchors.fill: parent
    }
}