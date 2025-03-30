#include "GameModel.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QThread>
#include <QTimer>

GameModel::GameModel(QObject *parent)
    : QObject(parent)
    , m_userField({ShipLocation::Field(10, QVector<int>(10, 0)), {}})
    , m_enemyField({ShipLocation::Field(10, QVector<int>(10, 0)), {}})
    , m_gameIsActive(false) {}

void GameModel::placeShipsRandomly(ShipLocation& location) {
    clearField(location);

    QVector<int> shipSizes = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    for (int size : shipSizes) {
        generateRandomShip(size, location);
    }
}

void GameModel::placeShipsRandomlyUser() {
    m_gameIsActive = false;         // Расположили корабли - игра начинается заново
    placeShipsRandomly(m_userField);
}

void GameModel::generateRandomShip(int size, ShipLocation& location) {
    bool placed = false;
    while (!placed) {
        int x = QRandomGenerator::global()->bounded(10);
        int y = QRandomGenerator::global()->bounded(10);
        bool horizontal = QRandomGenerator::global()->bounded(2);

        if (horizontal) {
            if (x + size > 10) continue; // Корабль не помещается по горизонтали
            if (isAreaClear(x, y, size, horizontal, location)) {
                QVector<std::pair<int, int>> coordinates;
                for (int i = 0; i < size; ++i) {
                    location.field[x + i][y] = 1; // Размещаем корабль
                    coordinates.push_back({x + i, y});
                }

                fillEnemyShipsCoordinates(location, coordinates);

                placed = true;
            }
        } else {
            if (y + size > 10) continue; // Корабль не помещается по вертикали
            if (isAreaClear(x, y, size, horizontal, location)) {
                QVector<std::pair<int, int>> coordinates;
                for (int i = 0; i < size; ++i) {
                    location.field[x][y + i] = 1; // Размещаем корабль
                    coordinates.push_back({x, y + i});
                }

                fillEnemyShipsCoordinates(location, coordinates);

                placed = true;
            }
        }
    }
}

void GameModel::fillEnemyShipsCoordinates(ShipLocation& location, QVector<std::pair<int, int>>& coordinates) {
    for (const auto& coord : coordinates) {
        // Получаем текущую координату
        auto currentCoord = coord;

        // Создаем вектор для хранения пар
        QVector<std::pair<int, int>> pairs;

        // Добавляем все остальные координаты
        for (const auto& otherCoord : coordinates) {
            pairs.push_back(otherCoord);
        }

        // Добавляем в QMap
        location.mapShipsCoordinates[currentCoord] = pairs;
    }
}

bool GameModel::placeShip(int position, int size) {
    int x = position / 10;
    int y = position % 10;

    // Проверка возможности размещения корабля
    for (int i = 0; i < size; ++i) {
        if (m_userField.field[x][y + i] != 0) {
            return false; // Невозможно разместить корабль
        }
    }

    // Размещение корабля
    for (int i = 0; i < size; ++i) {
        m_userField.field[x][y + i] = 1; // 1 означает, что здесь находится корабль
    }

    return true;
}

QString GameModel::attackPosition(int position) {
    int x = position / 10;
    int y = position % 10;

    if (m_enemyField.field[x][y] == 1) {
        m_enemyField.field[x][y] = 2; // Ранен
        qDebug() << "Hit at:" << x << y;

        // Проверяем, уничтожен ли корабль
        if (isShipDestroyed(x, y, m_enemyField)) {
            qDebug() << "Ship destroyed!";
            markAreaAroundShip(x, y, m_enemyField);
        }

        if (areAllShipsDestroyed(m_enemyField)) {
            emit gameOver("Player");
            m_gameIsActive = false;
        }
        return "hit";
    } else {
        m_enemyField.field[x][y] = 3; // Промах
        qDebug() << "Miss at:" << x << y;
        return "miss";
    }
}

QString GameModel::enemyAttackPosition(int position) {
    int x = position / 10;
    int y = position % 10;

    if (m_userField.field[x][y] == 1) {
        m_userField.field[x][y] = 2; // Ранен
        qDebug() << "Enemy hit at:" << x << y;
        if (isShipDestroyed(x, y, m_userField)) {
            qDebug() << "User ship destroyed!";
            markAreaAroundShip(x, y, m_userField);
        }

        return "hit";
    } else {
        m_userField.field[x][y] = 3; // Промах
        qDebug() << "Enemy miss at:" << x << y;
        return "miss";
    }
}

void GameModel::startGame() {
    m_gameIsActive = true;
    m_enemyField.mapShipsCoordinates.clear();
    placeShipsRandomly(m_enemyField);
    m_attackedPositions.clear();
    enemyAI();
}

void GameModel::enemyAI() {
    emit enabledUserField(false);
    int position;
    do {
        qDebug() << "enemy attackes";
        int x = QRandomGenerator::global()->bounded(10);
        int y = QRandomGenerator::global()->bounded(10);
        position = x * 10 + y;
    } while (m_attackedPositions.contains(position)); // Проверяем, что позиция ещё не атакована

    // Запоминаем атакованную позицию
    m_attackedPositions.insert(position);

    // Атакуем выбранную позицию
    QString result = enemyAttackPosition(position);
    emit userFieldUpdated(position, result);

    if (areAllShipsDestroyed(m_userField)) {
        clearField(m_userField);    // На всякий случай
        clearField(m_enemyField);    // На всякий случай
        emit gameOver("Enemy");
        m_gameIsActive = false;
    } else if (result == "hit") {
//        QThread().sleep(2);
//        QTimer::singleShot(1000, this, SLOT(enemyAI()));
        enemyAI();
    }
    emit enabledUserField(true);
}

QStringList GameModel::getEnemyShipPositions() const  {
    return getShipPositions(m_enemyField);
}

QStringList GameModel::getUserShipPositions() const {
    return getShipPositions(m_userField);
}

QStringList GameModel::getFieldState() const {
    QStringList state;
//    const Field &field = isUserField ? m_userField : m_enemyField;

    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            int value = m_userField.field[x][y];
            if (value != 0) { // Если клетка не пустая
                state.append(QString("%1:%2").arg(x * 10 + y).arg(value));
            }
        }
    }
    return state;
}

bool GameModel::gameIsActive() const { return m_gameIsActive; }

void GameModel::clearField(ShipLocation& location) {
    for (auto &row : location.field) {
        std::fill(row.begin(), row.end(), 0); // Обнуляем каждую строку
    }
    location.mapShipsCoordinates.clear();
}

QStringList GameModel::getShipPositions(const ShipLocation& location) const {
    QStringList positions;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (location.field[x][y] == 1) { // 1 означает, что здесь находится корабль
                positions.append(QString::number(x * 10 + y));
            }
        }
    }
    return positions;
}

bool GameModel::areAllShipsDestroyed(const ShipLocation& location) {
    for (const auto &row : location.field) {
        for (int cell : row) {
            if (cell == 1) return false; // Есть живой корабль
        }
    }
    return true; // Все корабли уничтожены
}

bool GameModel::isAreaClear(int x, int y, int size, bool horizontal, ShipLocation& location) const  {
    // Определяем границы области вокруг корабля
    int startX = std::max(0, x - 1);
    int startY = std::max(0, y - 1);
    int endX = horizontal ? std::min(9, x + size) : std::min(9, x + 1);
    int endY = horizontal ? std::min(9, y + 1) : std::min(9, y + size);

    // Проверяем все клетки в этой области
    for (int i = startX; i <= endX; ++i) {
        for (int j = startY; j <= endY; ++j) {
            if (location.field[i][j] != 0) {
                return false; // Область занята
            }
        }
    }
    return true; // Область свободна
}

bool GameModel::isShipDestroyed(int x, int y, ShipLocation& location) const {
    auto coordinates = location.mapShipsCoordinates.find({x, y});
    if (coordinates != location.mapShipsCoordinates.end()) {
        for (auto& [x, y] : *coordinates) {
            if (location.field[x][y] != 2) {
                return false;
            }
        }
    }

    return true; // Корабль уничтожен
}

void GameModel::markAreaAroundShip(int x, int y, ShipLocation& location) {
    auto coordinates = location.mapShipsCoordinates.find({x, y});
    if (coordinates != location.mapShipsCoordinates.end()) {
        for (auto& [xx, yy] : *coordinates) {
            for (int i = -1; i <= 1; ++i) { // Проходим по строкам
                for (int j = -1; j <= 1; ++j) { // Проходим по столбцам
                    if (i == 0 && j == 0) {
                        continue; // Пропускаем саму точку
                    }

                    int newX = xx + i;
                    int newY = yy + j;

                    // Проверяем границы
                    if (newX >= 0 && newX < 10 && newY >= 0 && newY < 10 && location.field[newX][newY] != 2) {
                        location.field[newX][newY] = 3; // Заполняем 3 - промах (белая клетка)
                        int position = newX * 10 + newY;
//                        emit enemyTurn(position, "miss");
                        location == m_enemyField
                            ? emit enemyTurn(position, "miss")
                            : emit userFieldUpdated(position, "miss");
                    }
                }
            }
        }
    }
}
