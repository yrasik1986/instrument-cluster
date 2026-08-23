#pragma once

#include <QCanBusFrame>

#include "../model/VehicleData.h"

/**
 * @brief Парсер CAN-кадров для автомобильных данных
 *
 * Преобразует сырые CAN-кадры в структуру VehicleData.
 * Поддерживает два типа кадров:
 * - DriveData (ID 0x100): скорость, обороты, зажигание
 * - StatusData (ID 0x101): температура, топливо, статусы ламп
 *
 * Формат данных:
 * DriveData (5 байт):
 *   - [0-1] speed: uint16 LE, масштаб 0.01 (например 1234 = 12.34 км/ч)
 *   - [2-3] rpm: uint16 LE, об/мин
 *   - [4]   ignition: 0 = выкл, !0 = вкл
 *
 * StatusData (3 байта):
 *   - [0] coolantTemperature: uint8, °C
 *   - [1] fuel: uint8, %
 *   - [2] status: битовые флаги
 *       бит 0: левый поворотник
 *       бит 1: правый поворотник
 *       бит 2: дальний свет
 *       бит 3: Check Engine
 */
class CanParser
{
public:
    bool parse(const QCanBusFrame &frame, VehicleData &data);

private:
    bool parseDriveData(const QByteArray &payload, VehicleData &data);
    bool parseStatusData(const QByteArray &payload, VehicleData &data);

    static std::uint16_t readUInt16LE(const QByteArray &data,int offset);
};