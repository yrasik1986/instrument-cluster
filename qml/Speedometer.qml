import QtQuick

Item {
    id: root

    property real value: 0
    property real maximum: 220
    property string title: "km/h"

    readonly property real startAngle: -130
    readonly property real endAngle: 130

    readonly property real gaugeRadius:
        Math.min(width, height) * 0.39

    // =========================
    // Шкала
    // =========================

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

            // Основная дуга
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

            // Риски
            var tickCount = 22

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

    // =========================
    // Цифры 0...220
    // =========================

    Repeater {
        model: 23

        Text {
            required property int index

            property real tickValue:
                index * 10

            property real angle:
                root.startAngle +
                (root.endAngle - root.startAngle) *
                tickValue /
                root.maximum

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

            font.pixelSize: 13
            font.bold: true

            horizontalAlignment:
                Text.AlignHCenter

            verticalAlignment:
                Text.AlignVCenter
        }
    }

    // =========================
    // Стрелка
    // =========================

    Item {
        id: needle

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


        // Только верхняя половина стрелки.
        // Нижняя часть не закрывает цифровое значение.
        Rectangle {
            width: 5
            height: root.height * 0.22

            radius: 2

            color: "#e7e9ed"

            anchors.horizontalCenter:
                parent.horizontalCenter

            anchors.bottom:
                parent.verticalCenter

            anchors.bottomMargin:
                root.height * 0.02
        }

        // Центральная втулка
        Rectangle {
            width: 18
            height: 18

            radius: 9

            anchors.centerIn: parent

            color: "#e7e9ed"
        }
    }

    // =========================
    // Цифровая скорость
    // =========================

    Text {
        id: valueText

        anchors.horizontalCenter:
            parent.horizontalCenter

        anchors.verticalCenter:
            parent.verticalCenter

        anchors.verticalCenterOffset:
            root.height * 0.13

        text: root.value.toFixed(1)

        color: "white"

        font.pixelSize: 38
        font.bold: true

        horizontalAlignment:
            Text.AlignHCenter

        verticalAlignment:
            Text.AlignVCenter
    }

    // =========================
    // Единицы измерения
    // =========================

    Text {
        anchors.horizontalCenter:
            parent.horizontalCenter

        anchors.top:
            valueText.bottom

        anchors.topMargin: 4

        text: root.title

        color: "#808894"

        font.pixelSize: 16

        horizontalAlignment:
            Text.AlignHCenter
    }
}