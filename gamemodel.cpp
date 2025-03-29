#include "gamemodel.h"
#include <QDebug>
#include <QRandomGenerator>

GameModel::GameModel(QObject *parent)
    : QObject(parent)
    , m_userField(Field(10, QVector<int>(10, 0)))
    , m_enemyField(Field(10, QVector<int>(10, 0)))
    , m_gameIsActive(false) {}

void GameModel::placeShipsRandomly(Field& field) {
    for (auto &row : field) {
        std::fill(row.begin(), row.end(), 0); // Обнуляем каждую строку
    }
    QVector<int> shipSizes = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    for (int size : shipSizes) {
        generateRandomShip(size, field);
    }
}

void GameModel::placeShipsRandomlyUser() {
    m_gameIsActive = false;         // Расположили корабли - игра начинается заново
    placeShipsRandomly(m_userField);
}

void GameModel::generateRandomShip(int size, Field& field) {
    bool placed = false;
    while (!placed) {
        int x = QRandomGenerator::global()->bounded(10);
        int y = QRandomGenerator::global()->bounded(10);
        bool horizontal = QRandomGenerator::global()->bounded(2);

        if (horizontal) {
            if (x + size > 10) continue; // Корабль не помещается по горизонтали
            if (isAreaClear(x, y, size, horizontal, field)) {
                for (int i = 0; i < size; ++i) {
                    field[x + i][y] = 1; // Размещаем корабль
                }
                placed = true;
            }
        } else {
            if (y + size > 10) continue; // Корабль не помещается по вертикали
            if (isAreaClear(x, y, size, horizontal, field)) {
                for (int i = 0; i < size; ++i) {
                    field[x][y + i] = 1; // Размещаем корабль
                }
                placed = true;
            }
        }
    }
}

bool GameModel::placeShip(int position, int size) {
    int x = position / 10;
    int y = position % 10;

    // Проверка возможности размещения корабля
    for (int i = 0; i < size; ++i) {
        if (m_userField[x][y + i] != 0) {
            return false; // Невозможно разместить корабль
        }
    }

    // Размещение корабля
    for (int i = 0; i < size; ++i) {
        m_userField[x][y + i] = 1; // 1 означает, что здесь находится корабль
    }

    return true;
}

QString GameModel::attackPosition(int position) {
    int x = position / 10;
    int y = position % 10;

    if (m_enemyField[x][y] == 1) {
        m_enemyField[x][y] = 2; // Ранен
        qDebug() << "Hit at:" << x << y;
        if (areAllShipsDestroyed(m_enemyField)) {
            emit gameOver("Player");
            m_gameIsActive = false;
        }
        return "hit";
    } else {
        m_enemyField[x][y] = 3; // Промах
        qDebug() << "Miss at:" << x << y;
        return "miss";
    }
}

QString GameModel::enemyAttackPosition(int position) {
    int x = position / 10;
    int y = position % 10;

    if (m_userField[x][y] == 1) {
        m_userField[x][y] = 2; // Ранен
        qDebug() << "Enemy hit at:" << x << y;
        return "hit";
    } else {
        m_userField[x][y] = 3; // Промах
        qDebug() << "Enemy miss at:" << x << y;
        return "miss";
    }
}

void GameModel::startGame() {
    m_gameIsActive = true;
    placeShipsRandomly(m_enemyField);
    m_attackedPositions.clear();
    enemyAI();
}

void GameModel::enemyAI() {
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
        emit gameOver("Enemy");
        m_gameIsActive = false;
    } else if (result == "hit") {
        emit enemyTurn(position);
        enemyAI();
    } else {
        emit enemyTurn(position);
    }
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
            int value = m_userField[x][y];
            if (value != 0) { // Если клетка не пустая
                state.append(QString("%1:%2").arg(x * 10 + y).arg(value));
            }
        }
    }
    return state;
}

bool GameModel::gameIsActive() const { return m_gameIsActive; }

QStringList GameModel::getShipPositions(const QVector<QVector<int> > &field) const {
    QStringList positions;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (field[x][y] == 1) { // 1 означает, что здесь находится корабль
                positions.append(QString::number(x * 10 + y));
            }
        }
    }
    return positions;
}

bool GameModel::areAllShipsDestroyed(const Field &field) {
    for (const auto &row : field) {
        for (int cell : row) {
            if (cell == 1) return false; // Есть живой корабль
        }
    }
    return true; // Все корабли уничтожены
}

bool GameModel::isAreaClear(int x, int y, int size, bool horizontal, Field& field) const  {
    // Определяем границы области вокруг корабля
    int startX = std::max(0, x - 1);
    int startY = std::max(0, y - 1);
    int endX = horizontal ? std::min(9, x + size) : std::min(9, x + 1);
    int endY = horizontal ? std::min(9, y + 1) : std::min(9, y + size);

    // Проверяем все клетки в этой области
    for (int i = startX; i <= endX; ++i) {
        for (int j = startY; j <= endY; ++j) {
            if (field[i][j] != 0) {
                return false; // Область занята
            }
        }
    }
    return true; // Область свободна
}
