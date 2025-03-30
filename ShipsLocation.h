#include <QMap>
#include <QVector>

#pragma once

struct ShipLocation {
    using Field = QVector<QVector<int>>;
    using MapShipCoordinate = QMap<std::pair<int, int>, QVector<std::pair<int, int>>>;

    Field field{};
    // Хранит ключ - координата корабля, вектор - список координат этого же корабля
    // Например, для корабля [1,1][1,2][1,3] - {[1,1] : [1,1],[1,2][1,3]}, {[1,2] : [1,1],[1,2][1,3]}, {[1,3] : [1,1],[1,2][1,3]}
    MapShipCoordinate mapShipsCoordinates{};

    bool operator == (const ShipLocation& location) const {
        return field == location.field;
    }
    bool operator != (const ShipLocation& location) const {
        return !(location == *this);
    }
};
