#pragma once

#include <QObject>
#include <QTimer>
#include "VehicleData.h"

/**
 * @brief Модель данных авто с плавной анимацией стрелок
 *
 * Хранит состояние автомобиля, обновляется с CAN-шины,
 * предоставляет свойства для QML-привязок.
 *
 * @section animation_algorithm Алгоритм плавного движения стрелок
 *
 * Реализован гибридный подход:
 *
 * 1. **Экспоненциальное сглаживание (Exponential Smoothing)**
 *    - Формула: `V(t) = V(t-1) + (Target - V(t-1)) * α`
 *    - α - коэффициент сглаживания (0.08 для скорости, 0.15 для RPM)
 *    - Обеспечивает плавное приближение к целевому значению
 *
 * 2. **Разделение данных (Data Separation)**
 *    - `m_data` - точные значения с CAN (для цифровых индикаторов)
 *    - `m_animatedSpeed/Rpm` - сглаженные значения (для стрелок)
 *    - Позволяет отображать точные цифры и плавные стрелки одновременно
 *
 * 3. **Перекрытие анимаций (Animation Overlap)**
 *    - При поступлении нового CAN-кадра обновляется только целевое значение
 *    - Таймер анимации (16ms) продолжает работать независимо
 *    - Обеспечивает плавность даже при частых обновлениях данных
 *
 * @section parameters Параметры настройки
 *
 * | Параметр | Значение | Влияние |
 * |----------|----------|---------|
 * | SPEED_SMOOTHING | 0.08 | Медленное/плавное движение спидометра |
 * | RPM_SMOOTHING | 0.15 | Быстрое/отзывчивое движение тахометра |
 * | animationTimer | 16ms (60 FPS) | Частота обновления анимации |
 *
 * @section advantages Преимущества подхода
 *
 * - **Стабильность**: Работает при любом потоке CAN-данных
 * - **Производительность**: Минимальные вычисления (только сложение/умножение)
 * - **Реалистичность**: Движение стрелок как в реальных автомобилях
 * - **Разделение**: Точные цифры + плавные стрелки одновременно
 * - **Настраиваемость**: Легкая адаптация под разные стили приборов
 *
 * @note Алгоритм разработан на основе анализа движения стрелок
 *       в автомобилях премиум-класса (Mercedes, BMW, Audi)
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

    // Анимированные свойства для плавного движения стрелок
    Q_PROPERTY(double animatedSpeed READ animatedSpeed NOTIFY animatedSpeedChanged)
    Q_PROPERTY(int animatedRpm READ animatedRpm NOTIFY animatedRpmChanged)

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

    // Анимированные геттеры
    double animatedSpeed() const { return m_animatedSpeed; }
    int animatedRpm() const { return static_cast<int>(m_animatedRpm); }

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

    void animatedSpeedChanged();
    void animatedRpmChanged();

private slots:
    void updateAnimation();

private:
    VehicleData m_data;  // структура со всеми параметрами

    // Анимированные значения
    double m_animatedSpeed = 0.0;
    double m_animatedRpm = 0.0;

    // Целевые значения (последние данные с CAN)
    double m_targetSpeed = 0.0;
    double m_targetRpm = 0.0;

    QTimer m_animationTimer;  // таймер для плавного обновления

    // Коэффициенты сглаживания (чем меньше, тем плавнее)
    static constexpr double SPEED_SMOOTHING = 0.08;
    static constexpr double RPM_SMOOTHING = 0.15;
};