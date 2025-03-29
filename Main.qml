import QtQuick
import QtQuick.Window
import QtQuick.Controls
import GameLogicModule 1.0

Window {
    visible: true
    width: 400
    height: 800
    title: "Морской бой"

    GameLogic {
        id: gameLogic

        onGameOver: function(winner) {
            if (winner === "Player") {
                console.log("You win!");
                showWinner("Вы победили!")
            } else if (winner === "Enemy") {
                console.log("Enemy wins!");
                showWinner("Вы проиграли!")
            }
        }

        function showWinner(message) {
            infoText.text = message
            infoPopup.open()
        }

        onEnemyTurn: function(position) {
            console.log("Enemy turn:", position)
        }

        onUserFieldUpdated: function(position, result) {
            let cell = userGrid.children[position]
            if (result === "hit") {
                cell.color = "red"
            } else if (result === "miss") {
                cell.color = "white"
            }
        }

        function resetGridColors(grid, repeater, isUserField) {
            for (let i = 0; i < repeater.model; ++i) {
                let cell = repeater.itemAt(i)
                if (cell) {
                    // @disable-check M105
                    cell.color = grid === userGrid ? "lightblue" : "lightgray"
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
                            cell.color = "blue" // Корабль
                        } else if (state === 2) {
                            cell.color = "red" // Раненый корабль
                        } else if (state === 3) {
                            cell.color = "white" // Промах
                        }
                    }
                }
            }
        }
    }

    Column {
        spacing: 20
        anchors.centerIn: parent

        // Поле пользователя
        Grid {
            id: userGrid
            columns: 10
            rows: 10
            spacing: 2

            Repeater {
                id: userRepeater
                model: 100
                Rectangle {
                    width: 30
                    height: 30
                    color: "lightblue"
                    border.color: "black"

                    property int index: model.index // Индекс клетки

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (gameLogic.placeShip(index, 1)) {
                                parent.color = "blue" // Корабль размещен
                            }
                        }
                    }
                }
            }
        }

        // Поле противника
        Grid {
            id: enemyGrid
            columns: 10
            rows: 10
            spacing: 2

            Repeater {
                id: enemyRepeater
                model: 100
                Rectangle {
                    width: 30
                    height: 30
                    color: "lightgray"
                    border.color: "black"

                    property int index: model.index // Индекс клетки

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            let result = gameLogic.attackPosition(index)
                            if (result === "hit") {
                                parent.color = "red"
                            } else if (result === "miss") {
                                parent.color = "white"
                                gameLogic.enemyAI()
                            }
                        }
                    }
                }
            }
        }

        Button {
            text: "Начать игру"
            width: 200
            onClicked: {
                if (!gameLogic.gameIsActive()) {
                    gameLogic.resetGridColors(enemyGrid, enemyRepeater)
                    gameLogic.startGame()
                } else {
                    infoText.text = "Игра в процессе.\nРасположите корабли заново!"
                    infoPopup.open()
                }


                // Получаем позиции кораблей противника
//                let enemyShipPositions = gameLogic.getEnemyShipPositions()
//                for (let position of enemyShipPositions) {
//                    let cell = enemyGrid.children[parseInt(position)]
//                    if (cell) {
//                        cell.color = "blue" // Отображаем корабли противника
//                    }
//                }
            }
        }

        Button {
            text: "Расположить корабли"
            width: 200
            onClicked: {
                gameLogic.resetGridColors(userGrid, userRepeater)
                gameLogic.placeShipsRandomlyUser()
                // Получаем позиции кораблей игрока
                let userShipPositions = gameLogic.getUserShipPositions()
                for (let position of userShipPositions) {
                    let cell = userGrid.children[parseInt(position)]
                    if (cell) {
                        cell.color = "blue" // Отображаем корабли игрока
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
}
