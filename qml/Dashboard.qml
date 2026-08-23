import QtQuick
import QtQuick.Controls

Item {
    id: root

    property bool ignitionOn: vehicle.ignition

    Rectangle {
        anchors.fill: parent
        color: "#090b0f"
    }

    Row {
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 35

        Indicator {
            text: "◀"
            active: vehicle.leftTurn
            blink: true   // мигает
            color: "#30e878"
        }

        Indicator {
            text: "HIGH BEAM"
            active: vehicle.highBeam
            blink: false  // НЕ мигает
            color: "#55aaff"
        }

        Indicator {
            text: "ENGINE"
            active: vehicle.checkEngine
            blink: false  // НЕ мигает
            color: "#ff4c4c"
        }

        Indicator {
            text: "▶"
            active: vehicle.rightTurn
            blink: true   // мигает
            color: "#30e878"
        }
    }

    Speedometer {
        id: speedometer

        anchors.left: parent.left
        anchors.leftMargin: 45
        anchors.verticalCenter: parent.verticalCenter

        width: 410
        height: 410

        value: vehicle.ignition ? vehicle.animatedSpeed : 0
        maximum: 220
        title: "km/h"
    }

    Tachometer {
        id: tachometer

        anchors.right: parent.right
        anchors.rightMargin: 45
        anchors.verticalCenter: parent.verticalCenter

        width: 410
        height: 410

        value: vehicle.ignition ? vehicle.animatedRpm : 0
        maximum: 8000
        title: "RPM"
    }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 25

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: vehicle.ignition ? vehicle.speed.toFixed(1) : "0.0"
            color: "white"
            font.pixelSize: 42
            font.bold: true
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: vehicle.ignition ? vehicle.rpm.toString() : "0"
            color: "#aeb5bf"
            font.pixelSize: 22
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 40

            Column {
                spacing: 4

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "FUEL"
                    color: "#777f8a"
                    font.pixelSize: 13
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: vehicle.canConnected ? vehicle.fuel + "%" : "---"
                    color: vehicle.lowFuel ? "#ff5252" : "white"
                    font.pixelSize: 22
                    font.bold: true
                }
            }

            Column {
                spacing: 4

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "COOLANT"
                    color: "#777f8a"
                    font.pixelSize: 13
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: vehicle.canConnected ? vehicle.coolantTemperature + "°C" : "---"
                    color: vehicle.highTemperature ? "#ff5252" : "white"
                    font.pixelSize: 22
                    font.bold: true
                }
            }
        }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 30

        Rectangle {
            width: 120
            height: 30
            radius: 15
            color: vehicle.canConnected ? "#14351f" : "#401515"

            Text {
                anchors.centerIn: parent
                text: vehicle.canConnected ? "CAN OK" : "CAN LOST"
                color: vehicle.canConnected ? "#35e878" : "#ff5252"
                font.pixelSize: 13
                font.bold: true
            }
        }

        Rectangle {
            width: 120
            height: 30
            radius: 15
            color: vehicle.ignition ? "#26344a" : "#1b1d21"

            Text {
                anchors.centerIn: parent
                text: vehicle.ignition ? "IGNITION ON" : "IGNITION OFF"
                color: "#d5dbe3"
                font.pixelSize: 13
            }
        }

        Rectangle {
            visible: vehicle.lowFuel
            width: 120
            height: 30
            radius: 15
            color: "#4a3010"

            Text {
                anchors.centerIn: parent
                text: "LOW FUEL"
                color: "#ffb52e"
                font.pixelSize: 13
                font.bold: true
            }
        }

        Rectangle {
            visible: vehicle.highTemperature
            width: 140
            height: 30
            radius: 15
            color: "#481818"

            Text {
                anchors.centerIn: parent
                text: "HIGH TEMP"
                color: "#ff5252"
                font.pixelSize: 13
                font.bold: true
            }
        }
    }
}