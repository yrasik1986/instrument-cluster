import QtQuick

Item {
    id: root

    property real value: 0
    property real maximum: 8000
    property string title: "RPM"

    readonly property real startAngle: -130
    readonly property real endAngle: 130

    Canvas {
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")

            ctx.reset()

            var cx = width / 2
            var cy = height / 2

            var radius = Math.min(width, height) * 0.43

            ctx.beginPath()

            ctx.arc(
                cx,
                cy,
                radius,
                startAngle * Math.PI / 180,
                endAngle * Math.PI / 180
            )

            ctx.lineWidth = 18
            ctx.strokeStyle = "#252a31"
            ctx.stroke()

            var tickCount = 16

            for (var i = 0; i <= tickCount; ++i) {

                var angle =
                    (startAngle +
                     (endAngle - startAngle)
                     * i / tickCount)
                    * Math.PI / 180

                var outerRadius = radius
                var innerRadius = radius - 18

                var x1 =
                    cx + Math.cos(angle) * innerRadius

                var y1 =
                    cy + Math.sin(angle) * innerRadius

                var x2 =
                    cx + Math.cos(angle) * outerRadius

                var y2 =
                    cy + Math.sin(angle) * outerRadius

                ctx.beginPath()

                ctx.moveTo(x1, y1)
                ctx.lineTo(x2, y2)

                ctx.lineWidth =
                    i % 2 === 0 ? 3 : 1

                ctx.strokeStyle = "#d5dbe3"

                ctx.stroke()
            }
        }
    }

    Item {
        width: root.width
        height: root.height

        rotation: root.startAngle +
                  (root.endAngle - root.startAngle)
                  * Math.max(
                      0,
                      Math.min(root.value, root.maximum)
                    )
                  / root.maximum

        transformOrigin: Item.Center

        Behavior on rotation {
            NumberAnimation {
                duration: 80
                easing.type: Easing.OutQuad
            }
        }

        Rectangle {
            width: 5
            height: root.height * 0.34

            radius: 2

            color: "#e7e9ed"

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            anchors.verticalCenterOffset:
                -root.height * 0.17
        }

        Rectangle {
            width: 18
            height: 18

            radius: 9

            anchors.centerIn: parent

            color: "#e7e9ed"
        }
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter

        y: height * 0.64

        text: Math.round(root.value)

        color: "white"

        font.pixelSize: 38
        font.bold: true
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter

        y: height * 0.76

        text: root.title

        color: "#808894"

        font.pixelSize: 16
    }
}