import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: mainMenuWindow
    visible: true
    width: 400
    height: 300
    title: "Морской бой - Главное меню"
    color: "#2c3e50"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Button {
            text: "Лёгкий режим"
            width: 200
            background: Rectangle {
                color: "#4b7357"
                radius: 20
            }
            onClicked: {
                gameLogic.setHuntingMode(false)
                // Открываем игровое окно
                var gameWindow = Qt.createComponent("Main.qml")
                if (gameWindow.status === Component.Ready) {
                    var window = gameWindow.createObject()
                    window.show()
                    mainMenuWindow.close() // Закрываем главное меню
                } else {
                    console.error("Ошибка загрузки игрового окна:", gameWindow.errorString())
                }
            }
        }

        Button {
            text: "Сложный режим"
            width: 200
            background: Rectangle {
                color: "#4b7357"
                radius: 20
            }
            onClicked: {
                gameLogic.setHuntingMode(true)
                // Открываем игровое окно
                var gameWindow = Qt.createComponent("Main.qml")
                if (gameWindow.status === Component.Ready) {
                    var window = gameWindow.createObject()
                    window.show()
                    mainMenuWindow.close() // Закрываем главное меню
                } else {
                    console.error("Ошибка загрузки игрового окна:", gameWindow.errorString())
                }
            }
        }

        Button {
            text: "Выход"
            width: 200
            background: Rectangle {
                color: "#4b7357"
                radius: 20
            }
            onClicked: {
                Qt.quit() // Завершаем приложение
            }
        }
    }
}
