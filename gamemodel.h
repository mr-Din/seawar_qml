#pragma once

#include <QMap>
#include <QObject>
#include <QSet>
#include <QVector>

#include "ShipsLocation.h"

class GameModel : public QObject {
    Q_OBJECT
    struct HuntingMode {
        bool isEnabled{false};
        QVector<std::pair<int, int>> hitParts{};
        QVector<std::pair<int, int>> missParts{};
        void clear() {
            isEnabled = false;
            hitParts.clear();
            missParts.clear();
        }
    };

public:
    explicit GameModel(QObject *parent = nullptr);

    Q_INVOKABLE void placeShipsRandomly(ShipLocation& location);
    Q_INVOKABLE void placeShipsRandomlyUser();
    Q_INVOKABLE bool placeShip(int position, int size); // Размещение корабля пользователя
    Q_INVOKABLE QString attackPosition(int position);   // Атака пользователя
    Q_INVOKABLE QString enemyAttackPosition(int position);
    Q_INVOKABLE void startGame();                      // Начало игры
    Q_INVOKABLE void enemyAI();
    Q_INVOKABLE void enemyAIWithTimeOut();
    Q_INVOKABLE QStringList getEnemyShipPositions() const;
    Q_INVOKABLE QStringList getUserShipPositions() const;
    Q_INVOKABLE QStringList getFieldState() const;
    Q_INVOKABLE bool gameIsActive() const;
    Q_INVOKABLE void setHuntingMode(bool enabled);

signals:
    void enemyTurn(int position, QString result);                      // Ход противника
    void userTurn(int position, QString result);                      // Ход пользователя
    void enemyFieldUpdated(int position, QString result); // Обновление поля противника
    void userFieldUpdated(int position, QString result);  // Обновление поля пользователя
    void gameOver(QString winner); // Сигнал о завершении игры
    void enabledEnemyField(bool isEnabled);
    void showMessage(QString message);
    void soundBoom();

private:
    int nextPointToAttack() const;
    bool pointInsideField(int x, int y) const;
    void clearField(ShipLocation& location);
    QStringList getShipPositions(const ShipLocation& location) const;
    void generateRandomShip(int size, ShipLocation& location);
    void fillEnemyShipsCoordinates(ShipLocation& location, QVector<std::pair<int, int>>& coordinates);

    bool areAllShipsDestroyed(const ShipLocation& location);
    bool isAreaClear(int x, int y, int size, bool horizontal, ShipLocation& location) const;
    bool isShipDestroyed(int x, int y, ShipLocation& location) const;
    void markAreaAroundShip(int x, int y, ShipLocation& location);

private:
    ShipLocation m_userField;      // Поле пользователя
    ShipLocation m_enemyField;     // Поле противника
    QSet<int> m_attackedPositions;          // Множество атакованных позиций
    bool m_gameIsActive;                    // Игра в процессе
    HuntingMode m_huntingMode;
    bool m_withHuntingMode;
};
