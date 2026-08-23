#pragma once

#include <QObject>

#include "VehicleData.h"

class VehicleModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)

    Q_PROPERTY(int rpm READ rpm NOTIFY rpmChanged)

    Q_PROPERTY(int coolantTemperature READ coolantTemperature NOTIFY coolantTemperatureChanged)

    Q_PROPERTY(int fuel READ fuel NOTIFY fuelChanged)

    Q_PROPERTY(bool ignition READ ignition NOTIFY ignitionChanged)

    Q_PROPERTY(bool leftTurn READ leftTurn NOTIFY leftTurnChanged)

    Q_PROPERTY(bool rightTurn READ rightTurn NOTIFY rightTurnChanged)

    Q_PROPERTY(bool highBeam READ highBeam NOTIFY highBeamChanged)

    Q_PROPERTY(bool checkEngine READ checkEngine NOTIFY checkEngineChanged)

    Q_PROPERTY(bool canConnected READ canConnected NOTIFY canConnectedChanged)

    Q_PROPERTY(bool lowFuel READ lowFuel NOTIFY warningsChanged)

    Q_PROPERTY(bool highTemperature READ highTemperature NOTIFY warningsChanged)

public:
    explicit VehicleModel(QObject *parent = nullptr);

    double speed() const;
    int rpm() const;

    int coolantTemperature() const;
    int fuel() const;

    bool ignition() const;

    bool leftTurn() const;
    bool rightTurn() const;

    bool highBeam() const;
    bool checkEngine() const;

    bool canConnected() const;

    bool lowFuel() const;
    bool highTemperature() const;

    void updateDriveData( double speed, std::uint16_t rpm,bool ignition);

    void updateStatusData( std::uint8_t coolantTemperature, std::uint8_t fuel,bool leftTurn, bool rightTurn, bool highBeam,  bool checkEngine);

    void setCanLost();

signals:
    void speedChanged();
    void rpmChanged();

    void coolantTemperatureChanged();
    void fuelChanged();

    void ignitionChanged();

    void leftTurnChanged();
    void rightTurnChanged();

    void highBeamChanged();
    void checkEngineChanged();

    void canConnectedChanged();

    void warningsChanged();

private:
    VehicleData m_data;
};