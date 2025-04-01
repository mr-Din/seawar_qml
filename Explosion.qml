import QtQuick

Rectangle {
    id: explosion
    width: 0
    height: 0
    color: "#ff0000" // Ярко-жёлтый цвет для тестирования
    radius: 100
    opacity: 1
    anchors.centerIn: parent

    // Анимация для ширины
    NumberAnimation on width {
        from: 0
        to: 150 // Максимальная ширина
        duration: 500
        onRunningChanged: {
            console.log("Width animation running:", running);
        }
    }

    // Анимация для высоты
    NumberAnimation on height {
        from: 0
        to: 150 // Максимальная высота
        duration: 500
        onRunningChanged: {
            console.log("Height animation running:", running);
        }
    }

    // Анимация для изменения цвета
    ColorAnimation on color {
        from: "#ff0000" // Жёлтый цвет
        to: "#ffff00"   // Красный цвет
        duration: 500   // Длительность анимации
    }

    // Анимация для прозрачности
    NumberAnimation on opacity {
        from: 1
        to: 0
        duration: 500
        onRunningChanged: {
            console.log("Opacity animation running:", running);
            if (!running) {
                parent.z = 0
            }
        }
    }
}
