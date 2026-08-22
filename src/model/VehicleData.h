#pragma once

#include <cstdint>

struct VehicleData
{
    double speed = 0.0;
    std::uint16_t rpm = 0;

    std::uint8_t coolantTemperature = 0;
    std::uint8_t fuel = 0;

    bool ignition = false;

    bool leftTurn = false;
    bool rightTurn = false;
    bool highBeam = false;
    bool checkEngine = false;

    bool canConnected = false;
};