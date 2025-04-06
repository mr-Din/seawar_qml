import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtMultimedia
import GameLogicModule 1.0

Window {
    id: gameWindow
    visible: false
    width: 400
    height: 800
    title: "Морской бой"
    color: "#2c3e50"

    Connections {
        target: gameLogic

        function onGameOver(winner) {
            if (winner === "Player") {
                console.log("You win!");
                showWinner("Вы победили!")
            } else if (winner === "Enemy") {
                console.log("Enemy wins!");
                enemyGrid.enabled = false;
                showWinner("Вы проиграли!")
            }
        }

        function onEnemyTurn(position, result) {
            let cell = userGrid.children[position]
            if (result === "hit") {
                cell.color = "#e74c3c"
                bangSound.play()
                createExplosion(cell)
            } else if (result === "miss") {
                cell.color = "#898d91"
                waterDropSound.play()
                createSplash(cell)
            }
        }

        function onUserTurn(position, result) {
            let cell = enemyGrid.children[position]
            if (result === "hit") {
                cell.color = "#e74c3c"
                createExplosion(cell)
            } else if (result === "miss") {
                cell.color = "#898d91"
                createSplash(cell)
            }
        }

        function onEnemyFieldUpdated(position, result) {
            let cell = enemyGrid.children[position]
            if (result === "hit") {
                cell.color = "#e74c3c"
            } else if (result === "miss") {
                cell.color = "#898d91"
            }
        }

        function onUserFieldUpdated(position, result) {
            let cell = userGrid.children[position]
            if (result === "hit") {
                cell.color = "#e74c3c"
            } else if (result === "miss") {
                cell.color = "#898d91"
            }
        }

        function onEnabledEnemyField(enabled) {
            enemyGrid.enabled = enabled
            if (enabled) {
                enemyIsActive.close()
            } else {
                enemyIsActive.open()
            }
        }

        function onShowMessage(message) {
            boomSound.play()
            messageText.text = message; // Устанавливаем текст сообщения
            messagePopup.parent = userGrid;
            messagePopup.open(); // Показываем всплывающее окно
            messageTimer.restart(); // Запускаем таймер
        }

        function onSoundBoom() {
            boomSound.play()
        }
    }

    Column {
        spacing: 8
        anchors.centerIn: parent

        // Поле пользователя
        Item {
            id: userGrid
            width: 300
            height: 300

            Repeater {
                id: userRepeater
                model: 100
                Rectangle {
                    x: (model.index % 10) * 30 // Позиция по X
                    y: Math.floor(model.index / 10) * 30 // Позиция по Y
                    width: 30
                    height: 30
                    color: "#34495e" // Темно-синий цвет для пустых клеток
                    border.color: "#2c3e50"

                    property int index: model.index // Индекс клетки

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            if (gameLogic.placeShip(index, 1)) {
                                parent.color = "#08213b" // Синий цвет для кораблей
                            }
                        }
                    }
                }
            }
        }

        // Поле противника
        Item {
            id: enemyGrid
            width: 300
            height: 300

            Repeater {
                id: enemyRepeater
                model: 100
                Rectangle {
                    x: (model.index % 10) * 30 // Позиция по X
                    y: Math.floor(model.index / 10) * 30 // Позиция по Y
                    width: 30
                    height: 30
                    color: "#3a4c5e" // Темно-синий цвет для пустых клеток
                    border.color: "#2c3e50"

                    property int index: model.index // Индекс клетки

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            let result = gameLogic.attackPosition(index)
                            if (result === "hit") {
                                bangSound.play()
                            } else if (result === "miss") {
                                console.log("Where is sound effect!")
                                waterDropSound.play()
                                gameLogic.enemyAIWithTimeOut()
                            } else if (result === "doubleClick") {
                                //
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            spacing: 8 // Расстояние между кнопками
            anchors.horizontalCenter: parent.horizontalCenter // Центрирование по горизонтали
            width: parent.width // Растягиваем контейнер на всю ширину родителя
            Button {
                text: "Начать\nигру"
                background: Rectangle {
                    color: "#4b7357"
                    radius: 20
                }
                Layout.fillWidth: true // Растягиваем кнопку по ширине
                Layout.preferredWidth: 150 // Минимальная ширина кнопки
                onClicked: {
                    if (!gameLogic.gameIsActive()) {
                        resetGridColors(enemyGrid, enemyRepeater)
                        gameLogic.startGame()
                        enemyGrid.enabled = true;
                        userGrid.enabled = false;
                    } else {
                        infoText.text = "Игра в процессе.\nРасположите корабли заново!"
                        infoPopup.open()
                    }


                    // Получаем позиции кораблей противника
                    // let enemyShipPositions = gameLogic.getEnemyShipPositions()
                    // for (let position of enemyShipPositions) {
                    //     let cell = enemyGrid.children[parseInt(position)]
                    //     if (cell) {
                    //         cell.color = "#08213b" // Отображаем корабли противника
                    //     }
                    // }
                }
            }

            Button {
                text: "Расположить\nкорабли"
                background: Rectangle {
                    color: "#4b7357"
                    radius: 20
                }
                Layout.fillWidth: true // Растягиваем кнопку по ширине
                Layout.preferredWidth: 150 // Минимальная ширина кнопки

                onClicked: {
                    resetGridColors(userGrid, userRepeater)
                    gameLogic.placeShipsRandomlyUser()
                    // Получаем позиции кораблей игрока
                    let userShipPositions = gameLogic.getUserShipPositions()
                    for (let position of userShipPositions) {
                        let cell = userGrid.children[parseInt(position)]
                        if (cell) {
                            cell.color = "#08213b" // Отображаем корабли игрока
                        }
                    }
                }
            }

            Button {
                text: "Выход в\nменю"
                background: Rectangle {
                    color: "#4b7357"
                    radius: 20
                }
                Layout.fillWidth: true // Растягиваем кнопку по ширине
                Layout.preferredWidth: 150 // Минимальная ширина кнопки
                onClicked: {
                    // Закрываем текущее окно
                    gameWindow.close()

                    // Открываем главное меню
                    var mainMenu = Qt.createComponent("MainMenu.qml")
                    if (mainMenu.status === Component.Ready) {
                        var menuWindow = mainMenu.createObject()
                        menuWindow.show()
                    } else {
                        console.error("Ошибка загрузки главного меню:", mainMenu.errorString())
                    }
                }
            }
        }
    }

    Popup {
        id: infoPopup
        modal: true
        visible: false
        width: 300
        height: 100
        anchors.centerIn: parent

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                id: infoText
                text: "Игра завершена"
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
            }

            Button {
                text: "OK"
                onClicked: {
                    infoPopup.close()
                }
            }
        }
    }

    Popup {
        id: enemyIsActive
        modal: true
        parent: enemyGrid
        visible: false
        dim: false
        width: 250 // Начальная ширина (будет увеличиваться)
        height: 250 // Начальная высота (будет увеличиваться)

        anchors.centerIn: parent
        background: Rectangle {
            color: Qt.rgba(0, 0, 0, 0.5)
            radius: 10 // Округление углов
        }

        closePolicy: Popup.NoAutoClose

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "Enemy is attacking"
                font.family: "Calibri"
                font.pixelSize: 24
                color: "white"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    Popup {
        id: messagePopup
        modal: true
        visible: false
        dim: false
        width: 200
        height: 100
        anchors.centerIn: parent
        background: Rectangle {
            color: "transparent" // Прозрачный фон
        }

        closePolicy: Popup.NoAutoClose

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                id: messageText
                text: ""
                font.family: "Calibri"
                font.pixelSize: 75
                color: Qt.rgba(0, 0, 0, 0.5)
                horizontalAlignment: Text.AlignHCenter
            }

            Timer {
                id: messageTimer
                interval: 500 // Таймер для закрытия Popup
                onTriggered: {
                    messagePopup.close()
                }
            }
        }

        // При открытии Popup запускаем анимацию увеличения
        onVisibleChanged: {
            if (visible) {
                messageTimer.restart() // Перезапускаем таймер
            }
        }
    }

    Loader {
        id: explosionLoader
        active: false
    }

    SoundEffect {
        id: waterDropSound
        source: "qrc:/media/kaplya_2.wav"
        volume: 1.0
    }

    SoundEffect {
        id: bangSound
        source: "qrc:/media/boom_1.wav"
        volume: 1.0
    }

    SoundEffect {
        id: boomSound
        source: "qrc:/media/boom_2.wav"
        volume: 0.7
    }

    function createExplosion(cell) {
        if (!cell || !userGrid) {
            return
        }

        explosionLoader.active = true
        explosionLoader.sourceComponent = Qt.createComponent("Explosion.qml")

        if (explosionLoader.status === Component.Ready) {
            let explosionItem = explosionLoader.item
            cell.z = 100
            explosionItem.parent = cell // Родительский элемент - ячейка
            explosionItem.anchors.centerIn = cell // Привязываем к центру ячейки
        } else {
//            console.error("Ошибка загрузки взрыва:", explosionLoader.errorString)
        }
    }

    function createSplash(cell) {
        if (!cell || !userGrid) {
            return
        }

        explosionLoader.active = true
        explosionLoader.sourceComponent = Qt.createComponent("Splashing.qml")

        if (explosionLoader.status === Component.Ready) {
            let explosionItem = explosionLoader.item
            cell.z = 100
            explosionItem.parent = cell // Родительский элемент - ячейка
            explosionItem.anchors.centerIn = cell // Привязываем к центру ячейки
        } else {
//            console.error("Ошибка загрузки взрыва:", explosionLoader.errorString)
        }
    }

    function showWinner(message) {
        infoText.text = message
        infoPopup.open()
    }

    function resetGridColors(grid, repeater, isUserField) {
        for (let i = 0; i < repeater.model; ++i) {
            let cell = repeater.itemAt(i)
            if (cell) {
                // @disable-check M105
                cell.color = grid === userGrid ? "#34495e" : "#3a4c5e"
            }
        }

        if (isUserField) {
            let fieldState = gameLogic.getFieldState()
            for (let entry of fieldState) {
                let parts = entry.split(":")
                let position = parseInt(parts[0])
                let state = parseInt(parts[1])

                let cell = repeater.itemAt(position)
                if (cell) {
                    if (state === 1) {
                        cell.color = "#08213b" // Корабль
                    } else if (state === 2) {
                        cell.color = "#e74c3c" // Раненый корабль
                    } else if (state === 3) {
                        cell.color = "#898d91" // Промах
                    }
                }
            }
        }
    }
}
