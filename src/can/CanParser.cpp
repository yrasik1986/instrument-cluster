#include "CanParser.h"

#include <QtGlobal>

bool CanParser::parse(const QCanBusFrame &frame, VehicleData &data)
{
    const int id = frame.frameId();

    const QByteArray payload = frame.payload();

    switch (id)
    {
    case 0x100:
        return parse100(payload, data);

    case 0x101:
        return parse101(payload, data);

    default:
        return false;
    }
}

bool CanParser::parse100(
    const QByteArray &payload,
    VehicleData &data)
{
    if (payload.size() < 5)
        return false;

    const std::uint16_t rawSpeed =
        readUInt16LE(payload, 0);

    const std::uint16_t rpm =
        readUInt16LE(payload, 2);

    const std::uint8_t ignition =
        static_cast<std::uint8_t>(payload.at(4));

    data.speed = static_cast<double>(rawSpeed) * 0.01;
    data.rpm = rpm;
    data.ignition = ignition != 0;

    data.canConnected = true;

    return true;
}

bool CanParser::parse101(
    const QByteArray &payload,
    VehicleData &data)
{
    if (payload.size() < 3)
        return false;

    data.coolantTemperature =
        static_cast<std::uint8_t>(payload.at(0));

    data.fuel =
        static_cast<std::uint8_t>(payload.at(1));

    const std::uint8_t status =
        static_cast<std::uint8_t>(payload.at(2));

    data.leftTurn =
        (status & (1u << 0)) != 0;

    data.rightTurn =
        (status & (1u << 1)) != 0;

    data.highBeam =
        (status & (1u << 2)) != 0;

    data.checkEngine =
        (status & (1u << 3)) != 0;

    return true;
}

std::uint16_t CanParser::readUInt16LE(
    const QByteArray &data,
    int offset)
{
    const auto low =
        static_cast<std::uint8_t>(data.at(offset));

    const auto high =
        static_cast<std::uint8_t>(data.at(offset + 1));

    return static_cast<std::uint16_t>(
        low | (static_cast<std::uint16_t>(high) << 8)
        );
}