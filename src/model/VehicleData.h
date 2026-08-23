#pragma once

#include <cstdint>

/**
 * @brief Структура данных автомобиля, получаемых с CAN-шины
 */
struct VehicleData
{
    double speed = 0.0;                      ///< Скорость, км/ч
    std::uint16_t rpm = 0;                   ///< Обороты двигателя, об/мин

    std::uint8_t coolantTemperature = 0;     ///< Температура охлаждающей жидкости, °C
    std::uint8_t fuel = 0;                   ///< Уровень топлива, %

    bool ignition = false;                   ///< Состояние зажигания

    bool leftTurn = false;                   ///< Левый указатель поворота
    bool rightTurn = false;                  ///< Правый указатель поворота
    bool highBeam = false;                   ///< Дальний свет фар
    bool checkEngine = false;                ///< Индикатор неисправности двигателя

    bool canConnected = false;               ///< Флаг подключения к CAN-шине
};