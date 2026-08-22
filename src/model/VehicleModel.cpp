#include "VehicleModel.h"

#include <QtGlobal>

VehicleModel::VehicleModel(QObject *parent)
    : QObject(parent)
{
}

double VehicleModel::speed() const
{
    return m_data.speed;
}

int VehicleModel::rpm() const
{
    return m_data.rpm;
}

int VehicleModel::coolantTemperature() const
{
    return m_data.coolantTemperature;
}

int VehicleModel::fuel() const
{
    return m_data.fuel;
}

bool VehicleModel::ignition() const
{
    return m_data.ignition;
}

bool VehicleModel::leftTurn() const
{
    return m_data.leftTurn;
}

bool VehicleModel::rightTurn() const
{
    return m_data.rightTurn;
}

bool VehicleModel::highBeam() const
{
    return m_data.highBeam;
}

bool VehicleModel::checkEngine() const
{
    return m_data.checkEngine;
}

bool VehicleModel::canConnected() const
{
    return m_data.canConnected;
}

bool VehicleModel::lowFuel() const
{
    return m_data.fuel < 10;
}

bool VehicleModel::highTemperature() const
{
    return m_data.coolantTemperature > 105;
}

void VehicleModel::setData(const VehicleData &data)
{
    const bool speedChangedValue =
        !qFuzzyCompare(m_data.speed + 1.0,
                       data.speed + 1.0);

    const bool rpmChangedValue =
        m_data.rpm != data.rpm;

    const bool coolantChangedValue =
        m_data.coolantTemperature !=
        data.coolantTemperature;

    const bool fuelChangedValue =
        m_data.fuel != data.fuel;

    const bool ignitionChangedValue =
        m_data.ignition != data.ignition;

    const bool leftChangedValue =
        m_data.leftTurn != data.leftTurn;

    const bool rightChangedValue =
        m_data.rightTurn != data.rightTurn;

    const bool highBeamChangedValue =
        m_data.highBeam != data.highBeam;

    const bool checkEngineChangedValue =
        m_data.checkEngine != data.checkEngine;

    const bool canChangedValue =
        m_data.canConnected != data.canConnected;

    const bool oldLowFuel = lowFuel();
    const bool oldHighTemp = highTemperature();

    m_data = data;

    if (speedChangedValue)
        emit speedChanged();

    if (rpmChangedValue)
        emit rpmChanged();

    if (coolantChangedValue)
        emit coolantTemperatureChanged();

    if (fuelChangedValue)
        emit fuelChanged();

    if (ignitionChangedValue)
        emit ignitionChanged();

    if (leftChangedValue)
        emit leftTurnChanged();

    if (rightChangedValue)
        emit rightTurnChanged();

    if (highBeamChangedValue)
        emit highBeamChanged();

    if (checkEngineChangedValue)
        emit checkEngineChanged();

    if (canChangedValue)
        emit canConnectedChanged();

    if (oldLowFuel != lowFuel() ||
        oldHighTemp != highTemperature())
    {
        emit warningsChanged();
    }
}

void VehicleModel::setCanLost()
{
    VehicleData data = m_data;

    data.speed = 0.0;
    data.rpm = 0;

    data.coolantTemperature = 0;
    data.fuel = 0;

    data.leftTurn = false;
    data.rightTurn = false;
    data.highBeam = false;
    data.checkEngine = false;

    data.canConnected = false;

    setData(data);
}