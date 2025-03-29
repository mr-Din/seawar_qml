#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QObject>
#include <QSet>
#include <QVector>

class GameModel : public QObject {
    Q_OBJECT
    using Field = QVector<QVector<int>>;

public:
    explicit GameModel(QObject *parent = nullptr);

    Q_INVOKABLE void placeShipsRandomly(Field& field);
    Q_INVOKABLE void placeShipsRandomlyUser();
    Q_INVOKABLE bool placeShip(int position, int size); // Размещение корабля пользователя
    Q_INVOKABLE QString attackPosition(int position);   // Атака пользователя
    Q_INVOKABLE QString enemyAttackPosition(int position);
    Q_INVOKABLE void startGame();                      // Начало игры
    Q_INVOKABLE void enemyAI();
    Q_INVOKABLE QStringList getEnemyShipPositions() const;
    Q_INVOKABLE QStringList getUserShipPositions() const;
    Q_INVOKABLE QStringList getFieldState() const;
    Q_INVOKABLE bool gameIsActive() const;

signals:
    void enemyTurn(int position);                      // Ход противника
    void userFieldUpdated(int position, QString result); // Обновление поля пользователя
    void gameOver(QString winner); // Сигнал о завершении игры

private:
    Field m_userField;      // Поле пользователя
    Field m_enemyField;     // Поле противника
    QSet<int> m_attackedPositions;          // Множество атакованных позиций
    bool m_gameIsActive;                    // Игра в процессе

    QStringList getShipPositions(const Field& field) const;
    void generateRandomShip(int size, Field& field);

    bool areAllShipsDestroyed(const Field &field);
    bool isAreaClear(int x, int y, int size, bool horizontal, Field& field) const;
};

#endif // GAMEMODEL_H
