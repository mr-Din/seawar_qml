import QtQuick

Rectangle {
    id: splashing
    width: 50
    height: 50
    color: "#07c5f0"
    radius: 100
    opacity: 1
    anchors.centerIn: parent

    // Анимация для ширины
    NumberAnimation on width {
        from: 50
        to: 0
        duration: 500
        onRunningChanged: {
            console.log("Width animation running:", running);
        }
    }

    // Анимация для высоты
    NumberAnimation on height {
        from: 50
        to: 0 // Максимальная высота
        duration: 500
        onRunningChanged: {
            console.log("Height animation running:", running);
        }
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
//                explosion.destroy(); // Удаляем вспышку после завершения анимации
            }
        }
    }
}
