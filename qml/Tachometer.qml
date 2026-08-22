import QtQuick

Item {
    id: root

    property real value: 0
    property real maximum: 8000
    property string title: "RPM"

    readonly property real startAngle: -130
    readonly property real endAngle: 130

    readonly property real gaugeRadius:
        Math.min(width, height) * 0.39

    Canvas {
        id: canvas

        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")

            ctx.reset()

            var cx = width / 2
            var cy = height / 2

            var radius = root.gaugeRadius

            var start =
                (root.startAngle - 90) *
                Math.PI / 180

            var end =
                (root.endAngle - 90) *
                Math.PI / 180

            // =========================
            // Основная дуга
            // =========================

            ctx.beginPath()

            ctx.arc(
                cx,
                cy,
                radius,
                start,
                end
            )

            ctx.lineWidth = 18
            ctx.strokeStyle = "#252a31"
            ctx.stroke()

            // =========================
            // Риски
            // =========================

            var tickCount = 16

            for (var i = 0; i <= tickCount; ++i) {

                var angle =
                    start +
                    (end - start) *
                    i / tickCount

                var outerRadius = radius
                var innerRadius = radius - 18

                var x1 =
                    cx +
                    Math.cos(angle) *
                    innerRadius

                var y1 =
                    cy +
                    Math.sin(angle) *
                    innerRadius

                var x2 =
                    cx +
                    Math.cos(angle) *
                    outerRadius

                var y2 =
                    cy +
                    Math.sin(angle) *
                    outerRadius

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

    // =================================
    // Цифры 0...8
    // 1 = 1000 RPM
    // =================================

    Repeater {
        model: 9

        Text {
            required property int index

            property real tickValue:
                index

            property real angle:
                root.startAngle +
                (root.endAngle - root.startAngle) *
                tickValue /
                8

            property real labelRadius:
                root.gaugeRadius - 43

            width: 30
            height: 20

            x:
                root.width / 2 +
                Math.cos(
                    (angle - 90) *
                    Math.PI / 180
                ) * labelRadius -
                width / 2

            y:
                root.height / 2 +
                Math.sin(
                    (angle - 90) *
                    Math.PI / 180
                ) * labelRadius -
                height / 2

            text: tickValue.toString()

            color: "#d5dbe3"

            font.pixelSize: 14
            font.bold: true

            horizontalAlignment:
                Text.AlignHCenter

            verticalAlignment:
                Text.AlignVCenter
        }
    }

    // =================================
    // Стрелка
    // =================================

    Item {
        width: root.width
        height: root.height

        rotation:
            root.startAngle +
            (root.endAngle - root.startAngle) *
            Math.max(
                0,
                Math.min(
                    root.value,
                    root.maximum
                )
            ) /
            root.maximum

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

            anchors.horizontalCenter:
                parent.horizontalCenter

            anchors.verticalCenter:
                parent.verticalCenter

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

    // =================================
    // Цифровое значение RPM
    // =================================

    Text {
        anchors.horizontalCenter:
            parent.horizontalCenter

        y: height * 0.64

        text: Math.round(root.value)

        color: "white"

        font.pixelSize: 38
        font.bold: true
    }

    // =================================
    // RPM
    // =================================

    Text {
        anchors.horizontalCenter:
            parent.horizontalCenter

        y: height * 0.76

        text: root.title

        color: "#808894"

        font.pixelSize: 16
    }
}