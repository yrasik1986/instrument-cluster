#pragma once

#include <QObject>
#include "VehicleData.h"

/**
 * Модель данных авто. Хранит всё состояние, обновляется с CAN-шины,
 * шлёт сигналы для QML-привязок.
 */
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

    // Геттеры
    double speed() const { return m_data.speed; }
    int rpm() const { return m_data.rpm; }
    int coolantTemperature() const { return m_data.coolantTemperature; }
    int fuel() const { return m_data.fuel; }

    bool ignition() const { return m_data.ignition; }
    bool leftTurn() const { return m_data.leftTurn; }
    bool rightTurn() const { return m_data.rightTurn; }
    bool highBeam() const { return m_data.highBeam; }
    bool checkEngine() const { return m_data.checkEngine; }
    bool canConnected() const { return m_data.canConnected; }

    // Вычисляемые состояния
    bool lowFuel() const;
    bool highTemperature() const;

    // Пришла посылка с CAN по движению
    void updateDriveData(double speed, std::uint16_t rpm, bool ignition);

    // Пришла посылка с CAN по статусам
    void updateStatusData(std::uint8_t coolantTemperature, std::uint8_t fuel,
                          bool leftTurn, bool rightTurn, bool highBeam, bool checkEngine);

    // CAN пропал - сбрасываем всё в ноль
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
    void warningsChanged();  // lowFuel или highTemperature поменялись

private:
    VehicleData m_data;  // структура со всеми параметрами
};