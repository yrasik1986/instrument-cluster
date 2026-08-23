#pragma once

#include <QCanBusFrame>

#include "../model/VehicleData.h"

class CanParser
{
public:
    bool parse(const QCanBusFrame &frame, VehicleData &data);

private:
    bool parse100(const QByteArray &payload, VehicleData &data);
    bool parse101(const QByteArray &payload, VehicleData &data);

    static std::uint16_t readUInt16LE(const QByteArray &data,int offset);
};