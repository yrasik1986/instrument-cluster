#include "VehicleModel.h"

#include <QtGlobal>


namespace
{
// Пороговые значения для предупреждений
constexpr int LOW_FUEL_THRESHOLD = 10;        // %
constexpr int HIGH_TEMP_THRESHOLD = 105;      // °C
}

VehicleModel::VehicleModel(QObject *parent)
    : QObject(parent)
{
}

bool VehicleModel::lowFuel() const
{
    return m_data.canConnected &&  m_data.fuel < LOW_FUEL_THRESHOLD;
}

bool VehicleModel::highTemperature() const
{
    return m_data.canConnected && m_data.coolantTemperature > HIGH_TEMP_THRESHOLD;
}

void VehicleModel::updateDriveData(double speed, std::uint16_t rpm, bool ignition)
{
    const auto old = m_data;

    m_data.speed = speed;
    m_data.rpm = rpm;
    m_data.ignition = ignition;
    m_data.canConnected = true;

    if (!qFuzzyCompare(old.speed, speed)) emit speedChanged();
    if (old.rpm != rpm) emit rpmChanged();
    if (old.ignition != ignition) emit ignitionChanged();
    if (!old.canConnected) emit canConnectedChanged();
}

void VehicleModel::updateStatusData(uint8_t temp, uint8_t fuel,
                                    bool left, bool right, bool high, bool engine)
{
    const auto old = m_data;

    m_data.coolantTemperature = temp;
    m_data.fuel = fuel;
    m_data.leftTurn = left;
    m_data.rightTurn = right;
    m_data.highBeam = high;
    m_data.checkEngine = engine;
    m_data.canConnected = true;

    if (old.coolantTemperature != temp) emit coolantTemperatureChanged();
    if (old.fuel != fuel) emit fuelChanged();
    if (old.leftTurn != left) emit leftTurnChanged();
    if (old.rightTurn != right) emit rightTurnChanged();
    if (old.highBeam != high) emit highBeamChanged();
    if (old.checkEngine != engine) emit checkEngineChanged();
    if (!old.canConnected) emit canConnectedChanged();

    const bool warnChanged= (lowFuel() != (old.fuel < LOW_FUEL_THRESHOLD && old.canConnected)) ||
                                 (highTemperature() != (old.coolantTemperature > HIGH_TEMP_THRESHOLD && old.canConnected));
    if (warnChanged) emit warningsChanged();
}


void VehicleModel::setCanLost()
{
    if (!m_data.canConnected) return;

    const auto old = m_data;
    m_data = {};

    if (!qFuzzyIsNull(old.speed)) emit speedChanged();
    if (old.rpm) emit rpmChanged();
    if (old.coolantTemperature) emit coolantTemperatureChanged();
    if (old.fuel) emit fuelChanged();
    if (old.leftTurn || old.rightTurn || old.highBeam || old.checkEngine) {
        if (old.leftTurn) emit leftTurnChanged();
        if (old.rightTurn) emit rightTurnChanged();
        if (old.highBeam) emit highBeamChanged();
        if (old.checkEngine) emit checkEngineChanged();
    }
    emit canConnectedChanged();
    emit warningsChanged();
}