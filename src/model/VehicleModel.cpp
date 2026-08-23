#include "VehicleModel.h"
#include <QtGlobal>
#include <QElapsedTimer>

namespace
{
// Пороговые значения для предупреждений
constexpr int LOW_FUEL_THRESHOLD = 10;        // %
constexpr int HIGH_TEMP_THRESHOLD = 105;      // °C
}

VehicleModel::VehicleModel(QObject *parent)
    : QObject(parent)
{
    // Запускаем таймер анимации ~60 FPS
    m_animationTimer.setInterval(16); // ~60 Hz
    connect(&m_animationTimer, &QTimer::timeout, this, &VehicleModel::updateAnimation);
    m_animationTimer.start();
}

bool VehicleModel::lowFuel() const
{
    return m_data.canConnected && m_data.fuel < LOW_FUEL_THRESHOLD;
}

bool VehicleModel::highTemperature() const
{
    return m_data.canConnected && m_data.coolantTemperature > HIGH_TEMP_THRESHOLD;
}

void VehicleModel::updateDriveData(double speed, std::uint16_t rpm, bool ignition)
{
    const auto old = m_data;

    // Сохраняем целевые значения
    m_targetSpeed = speed;
    m_targetRpm = static_cast<double>(rpm);

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

    const bool warnChanged = (lowFuel() != (old.fuel < LOW_FUEL_THRESHOLD && old.canConnected)) ||
                             (highTemperature() != (old.coolantTemperature > HIGH_TEMP_THRESHOLD && old.canConnected));
    if (warnChanged) emit warningsChanged();
}

void VehicleModel::setCanLost()
{
    if (!m_data.canConnected) return;

    const auto old = m_data;
    m_data = {};

    // Сбрасываем целевые значения
    m_targetSpeed = 0.0;
    m_targetRpm = 0.0;

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

void VehicleModel::updateAnimation()
{
    bool changed = false;

    // Плавное обновление скорости (экспоненциальное сглаживание)
    const double speedDiff = m_targetSpeed - m_animatedSpeed;
    if (std::abs(speedDiff) > 0.01) {
        m_animatedSpeed += speedDiff * SPEED_SMOOTHING;
        changed = true;
    } else if (m_animatedSpeed != m_targetSpeed) {
        m_animatedSpeed = m_targetSpeed;  // доводим до точного значения
        changed = true;
    }

    // Плавное обновление оборотов
    const double rpmDiff = m_targetRpm - m_animatedRpm;
    if (std::abs(rpmDiff) > 0.5) {
        m_animatedRpm += rpmDiff * RPM_SMOOTHING;
        changed = true;
    } else if (m_animatedRpm != m_targetRpm) {
        m_animatedRpm = m_targetRpm;
        changed = true;
    }

    if (changed) {
        emit animatedSpeedChanged();
        emit animatedRpmChanged();
    }
}