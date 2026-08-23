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
    return m_data.canConnected &&  m_data.fuel < 10;
}

bool VehicleModel::highTemperature() const
{
    return m_data.canConnected && m_data.coolantTemperature > 105;
}


void VehicleModel::updateDriveData(double speed, std::uint16_t rpm, bool ignition)
{
    const bool speedChangedValue =!qFuzzyCompare( m_data.speed + 1.0,speed + 1.0);

    const bool rpmChangedValue = m_data.rpm != rpm;

    const bool ignitionChangedValue = m_data.ignition != ignition;

    const bool canChangedValue = !m_data.canConnected;

    m_data.speed = speed;
    m_data.rpm = rpm;
    m_data.ignition = ignition;
    m_data.canConnected = true;

    if (speedChangedValue)
    {
         emit speedChanged();
    }


    if (rpmChangedValue)
    {
        emit rpmChanged();
    }


    if (ignitionChangedValue)
    {
        emit ignitionChanged();
    }


    if (canChangedValue)
    {
        emit canConnectedChanged();
    }
}


void VehicleModel::updateStatusData(std::uint8_t coolantTemperature,std::uint8_t fuel, bool leftTurn, bool rightTurn,bool highBeam, bool checkEngine)
{
    const bool coolantChangedValue = m_data.coolantTemperature != coolantTemperature;

    const bool fuelChangedValue = m_data.fuel != fuel;

    const bool leftChangedValue =  m_data.leftTurn != leftTurn;

    const bool rightChangedValue = m_data.rightTurn != rightTurn;

    const bool highBeamChangedValue = m_data.highBeam != highBeam;

    const bool checkEngineChangedValue =  m_data.checkEngine != checkEngine;

    const bool canChangedValue = !m_data.canConnected;

    const bool oldLowFuel = lowFuel();

    const bool oldHighTemperature = highTemperature();

    m_data.coolantTemperature =  coolantTemperature;

    m_data.fuel = fuel;

    m_data.leftTurn = leftTurn;

    m_data.rightTurn =  rightTurn;

    m_data.highBeam = highBeam;

    m_data.checkEngine = checkEngine;

    m_data.canConnected = true;

    if (coolantChangedValue)
    {
        emit coolantTemperatureChanged();
    }

    if (fuelChangedValue)
    {
        emit fuelChanged();
    }


    if (leftChangedValue)
    {
        emit leftTurnChanged();
    }


    if (rightChangedValue)
    {
        emit rightTurnChanged();
    }


    if (highBeamChangedValue)
    {
        emit highBeamChanged();
    }


    if (checkEngineChangedValue)
    {
        emit checkEngineChanged();
    }


    if (canChangedValue)
    {
        emit canConnectedChanged();
    }


    if (oldLowFuel != lowFuel() || oldHighTemperature != highTemperature())
    {
        emit warningsChanged();
    }
}


void VehicleModel::setCanLost()
{
    const bool speedChangedValue = !qFuzzyIsNull(m_data.speed);

    const bool rpmChangedValue = m_data.rpm != 0;

    const bool coolantChangedValue = m_data.coolantTemperature != 0;

    const bool fuelChangedValue =  m_data.fuel != 0;

    const bool leftChangedValue =  m_data.leftTurn;

    const bool rightChangedValue =  m_data.rightTurn;

    const bool highBeamChangedValue = m_data.highBeam;

    const bool checkEngineChangedValue =  m_data.checkEngine;

    const bool wasConnected =  m_data.canConnected;

    const bool oldLowFuel = lowFuel();

    const bool oldHighTemperature =  highTemperature();

    m_data.speed = 0.0;
    m_data.rpm = 0;

    m_data.coolantTemperature = 0;
    m_data.fuel = 0;

    m_data.leftTurn = false;
    m_data.rightTurn = false;
    m_data.highBeam = false;
    m_data.checkEngine = false;

    m_data.canConnected = false;

    if (speedChangedValue)
    {
        emit speedChanged();
    }

    if (rpmChangedValue)
    {
        emit rpmChanged();
    }


    if (coolantChangedValue)
    {
        emit coolantTemperatureChanged();
    }


    if (fuelChangedValue)
    {
        emit fuelChanged();
    }


    if (leftChangedValue)
    {
        emit leftTurnChanged();
    }


    if (rightChangedValue)
    {
        emit rightTurnChanged();
    }


    if (highBeamChangedValue)
    {
        emit highBeamChanged();
    }


    if (checkEngineChangedValue)
    {
        emit checkEngineChanged();
    }


    if (wasConnected)
    {
        emit canConnectedChanged();
    }


    if (oldLowFuel != lowFuel() || oldHighTemperature != highTemperature())
    {
        emit warningsChanged();
    }
}