#include <QCoreApplication>
#include <QTimer>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <random>

class CanGenerator : public QObject
{
    Q_OBJECT

public:
    explicit CanGenerator(QObject *parent = nullptr)
        : QObject(parent)
        , m_rng(std::random_device{}())
    {
        QString errorString;

        m_device = QCanBus::instance()->createDevice(QStringLiteral("socketcan"),
                                                     QStringLiteral("vcan0"),
                                                     &errorString);

        if (!m_device)
        {
            std::cerr << "CAN device error: " << errorString.toStdString() << '\n';
            return;
        }

        if (!m_device->connectDevice())
        {
            std::cerr << "CAN connection error: " << m_device->errorString().toStdString() << '\n';
            return;
        }

        connect(&m_timer, &QTimer::timeout, this, &CanGenerator::sendData);
        m_timer.start(20);

        std::cout << "========================================\n";
        std::cout << "CAN Generator - Dashboard Test Scenario\n";
        std::cout << "========================================\n\n";

        printScenario();
    }

private slots:
    void sendData()
    {
        m_frameCounter++;
        m_stepTimer++;
        const int frameMs = m_stepTimer * 20;

        // Проверка потери CAN в конце сценария
        if (m_scenarioStep == 8) {
            const int canLostDuration = 150;
            const int canLostStart = 100;

            if (m_stepTimer >= canLostStart && m_stepTimer < canLostStart + canLostDuration) {
                m_canLost = true;
                if (m_stepTimer == canLostStart) {
                    std::cout << "\n[!!!] CAN LOST! (3 seconds)\n";
                }
                return;
            } else if (m_stepTimer >= canLostStart + canLostDuration) {
                m_canLost = false;
                if (m_stepTimer == canLostStart + canLostDuration) {
                    std::cout << "[OK] CAN restored\n";
                    std::cout << "========================================\n";
                    std::cout << "Scenario complete - Restarting...\n";
                    std::cout << "========================================\n\n";
                }
                if (m_stepTimer >= canLostStart + canLostDuration + 50) {
                    m_scenarioStep = 0;
                    m_stepTimer = 0;
                    m_loopCounter++;
                    m_highBeamOn = false;
                    m_highBeam = false;
                    std::cout << "=== LOOP " << m_loopCounter << " ===\n";
                    printScenario();
                    return;
                }
            }
        }

        switch (m_scenarioStep)
        {
        case 0: // 0-2с: Зажигание выключено
        {
            m_ignitionOn = false;
            m_speed = 0.0;
            m_rpm = 0;
            m_fuel = 50;
            m_coolant = 30;
            m_leftTurn = false;
            m_rightTurn = false;
            m_highBeam = false;
            m_checkEngine = false;
            m_canLost = false;

            if (frameMs >= 2000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n[2s] IGNITION ON\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 1: // 2-4с: Включение зажигания + подсветка
        {
            m_ignitionOn = true;
            m_speed = 0.0;
            m_canLost = false;

            const double idleNoise = (std::sin(frameMs / 500.0) + 1.0) / 2.0;
            m_rpm = 800 + static_cast<int>(100.0 * idleNoise);

            m_fuel = 50;
            m_coolant = 30;

            // Включаем дальний свет ОДИН РАЗ и больше не трогаем
            if (!m_highBeamOn) {
                m_highBeam = true;
                m_highBeamOn = true;
                std::cout << "[HIGH BEAM] ON (once)\n";
            }

            // Для проверки лампочек мигаем только поворотниками
            // Дальний свет и Check Engine просто горят
            const bool blinkState = (frameMs / 500) % 2 == 0;
            m_leftTurn = blinkState;
            m_rightTurn = blinkState;
            m_checkEngine = true; // Просто горит, не мигает

            if (frameMs % 1000 < 20) {
                std::cout << "[Idle] RPM: " << m_rpm << "\r";
                std::cout.flush();
            }

            if (frameMs >= 2000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n\n[4s] All indicators ON - checking bulbs\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 2: // 4-19с: Заправка топлива
        {
            m_ignitionOn = true;
            m_speed = 0.0;
            m_canLost = false;
            m_checkEngine = false;

            const double idleNoise = (std::sin(frameMs / 400.0 + 1.0) + 1.0) / 2.0;
            m_rpm = 800 + static_cast<int>(150.0 * idleNoise);

            const double progress = std::min(1.0, frameMs / 15000.0);
            m_fuel = 50 + static_cast<int>(50.0 * progress);
            m_coolant = 30 + static_cast<int>(10.0 * progress);

            // Поворотники мигают по 3 секунды каждый
            if (frameMs >= 3000 && frameMs < 6000) {
                // Левый мигает
                const bool blinkState = (frameMs / 500) % 2 == 0;
                m_leftTurn = blinkState;
                m_rightTurn = false;
            } else if (frameMs >= 6000 && frameMs < 9000) {
                // Правый мигает
                const bool blinkState = (frameMs / 500) % 2 == 0;
                m_leftTurn = false;
                m_rightTurn = blinkState;
            } else {
                m_leftTurn = false;
                m_rightTurn = false;
            }

            // m_highBeam НЕ МЕНЯЕМ - он уже true

            if (frameMs % 500 < 20) {
                std::cout << "[Fuel] " << m_fuel << "%\r";
                std::cout.flush();
            }

            if (frameMs >= 15000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n\n[19s] Fuel: " << m_fuel << "% - Full tank\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 3: // 19-29с: Прогрев
        {
            m_ignitionOn = true;
            m_speed = 0.0;
            m_fuel = 95;
            m_leftTurn = false;
            m_rightTurn = false;
            m_checkEngine = false;
            m_canLost = false;

            const double idleNoise = (std::sin(frameMs / 300.0) + std::sin(frameMs / 700.0)) / 2.0 + 0.5;
            m_rpm = 800 + static_cast<int>(200.0 * idleNoise);

            const double progress = std::min(1.0, frameMs / 10000.0);
            m_coolant = 30 + static_cast<int>(20.0 * progress);

            // m_highBeam НЕ МЕНЯЕМ

            if (frameMs % 1000 < 20) {
                std::cout << "[Warmup] Temp: " << m_coolant << "°C, RPM: " << m_rpm << "\r";
                std::cout.flush();
            }

            if (frameMs >= 10000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n\n[29s] Engine warm: " << m_coolant << "°C\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 4: // 29-33с: Газ в пол
        {
            m_ignitionOn = true;
            m_speed = 0.0;
            m_fuel = 93;
            m_coolant = 50;
            m_leftTurn = false;
            m_rightTurn = false;
            m_checkEngine = false;
            m_canLost = false;

            const double progress = std::min(1.0, frameMs / 4000.0);
            m_rpm = 800 + static_cast<int>(5200.0 * progress);

            // m_highBeam НЕ МЕНЯЕМ

            if (frameMs % 200 < 20) {
                std::cout << "[Rev] RPM: " << m_rpm << "\r";
                std::cout.flush();
            }

            if (frameMs >= 4000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n\n[33s] RPM: " << m_rpm << " - Ready to go!\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 5: // 33-41с: Разгон
        {
            m_ignitionOn = true;
            m_fuel = 90;
            m_leftTurn = false;
            m_rightTurn = false;
            m_checkEngine = false;
            m_canLost = false;

            const double progress = std::min(1.0, frameMs / 8000.0);
            m_speed = progress * 100.0;
            m_coolant = 50 + static_cast<int>(5.0 * (frameMs / 8000.0));

            double rpmBase = 800 + progress * 5200.0;
            if (frameMs % 800 < 100) {
                rpmBase *= 1.1;
            }
            m_rpm = static_cast<int>(rpmBase);

            // m_highBeam НЕ МЕНЯЕМ

            if (frameMs % 500 < 20) {
                std::cout << "[Accel] Speed: " << m_speed << " km/h, RPM: " << m_rpm << "\r";
                std::cout.flush();
            }

            if (frameMs >= 8000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n\n[41s] Speed: " << m_speed << " km/h - Cruising\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 6: // 41-56с: Круиз
        {
            m_ignitionOn = true;
            m_leftTurn = false;
            m_rightTurn = false;
            m_checkEngine = false;
            m_canLost = false;

            const double wave = (std::sin(frameMs / 2000.0) + 1.0) / 2.0;
            m_speed = 80.0 + 20.0 * wave;
            m_coolant = 55 + static_cast<int>(10.0 * (frameMs / 15000.0));
            m_fuel = 85 - static_cast<int>(10.0 * (frameMs / 15000.0));
            m_rpm = 2000 + static_cast<int>(500.0 * wave);

            // m_highBeam НЕ МЕНЯЕМ

            if (frameMs % 2000 < 20) {
                std::cout << "[Cruise] Speed: " << m_speed << " km/h, Temp: " << m_coolant << "°C\n";
            }

            if (frameMs >= 15000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n[56s] Entering emergency mode...\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 7: // 56-70с: Авария
        {
            m_ignitionOn = true;
            m_leftTurn = false;
            m_rightTurn = false;
            m_canLost = false;

            const double tempProgress = std::min(1.0, frameMs / 10000.0);
            m_coolant = 65 + static_cast<int>(55.0 * tempProgress);

            const double fuelProgress = std::min(1.0, frameMs / 12000.0);
            m_fuel = 85 - static_cast<int>(80.0 * fuelProgress);

            m_checkEngine = (m_coolant > 105);
            m_speed = 60.0 * (1.0 - frameMs / 14000.0);
            m_rpm = 1500 - static_cast<int>(700.0 * (frameMs / 14000.0));

            // m_highBeam НЕ МЕНЯЕМ - горит постоянно

            if (frameMs % 1000 < 20) {
                std::cout << "[EMERGENCY] Temp: " << m_coolant << "°C";
                std::cout << ", Fuel: " << m_fuel << "%";
                std::cout << ", Speed: " << m_speed << " km/h";
                if (m_checkEngine) std::cout << " [CHECK ENGINE!]";
                if (m_fuel < 10) std::cout << " [LOW FUEL!]";
                if (m_coolant > 105) std::cout << " [OVERHEAT!]";
                std::cout << "\n";
            }

            if (frameMs >= 14000) {
                m_scenarioStep++;
                m_stepTimer = 0;
                std::cout << "\n[70s] Emergency mode complete\n";
                std::cout << "----------------------------------------\n";
            }
            break;
        }

        case 8: // 70-76с: Выключение
        {
            if (frameMs < 2000) {
                m_ignitionOn = false;
                m_speed = 0.0;
                m_rpm = 0;
                m_fuel = 5;
                m_coolant = 120;
                m_leftTurn = false;
                m_rightTurn = false;
                m_highBeam = false; // ТУТ ВЫКЛЮЧАЕТСЯ
                m_checkEngine = true;
                m_canLost = false;

                if (frameMs == 0) {
                    std::cout << "\n[70s] IGNITION OFF, HIGH BEAM OFF\n";
                }
            }
            break;
        }

        default:
            break;
        }

        // Отправка CAN-кадров
        if (!m_canLost) {
            send100(m_speed, m_rpm, m_ignitionOn);  // 0x100 — 20 мс
            if (m_frameCounter % 5 == 0) {          // 0x101 — 100 мс
                send101(m_coolant, m_fuel, m_leftTurn, m_rightTurn,
                        m_highBeam, m_checkEngine);
            }
        }
    }

private:
    void send100(double speed, int rpm, bool ignition)
    {
        const int rawSpeed = static_cast<int>(std::clamp(speed, 0.0, 655.35) * 100.0);

        QByteArray data(8, 0);

        data[0] = static_cast<char>(rawSpeed & 0xFF);
        data[1] = static_cast<char>((rawSpeed >> 8) & 0xFF);
        data[2] = static_cast<char>(rpm & 0xFF);
        data[3] = static_cast<char>((rpm >> 8) & 0xFF);
        data[4] = ignition ? 1 : 0;

        QCanBusFrame frame(0x100, data);

        if (!m_device->writeFrame(frame))
        {
            std::cerr << "Failed to send 0x100\n";
        }
    }

    void send101(int coolant, int fuel, bool leftTurn, bool rightTurn,
                 bool highBeam, bool checkEngine)
    {
        QByteArray data(8, 0);

        data[0] = static_cast<char>(std::clamp(coolant, 0, 150));
        data[1] = static_cast<char>(std::clamp(fuel, 0, 100));

        std::uint8_t status = 0;
        if (leftTurn) status |= (1 << 0);
        if (rightTurn) status |= (1 << 1);
        if (highBeam) status |= (1 << 2);
        if (checkEngine) status |= (1 << 3);

        data[2] = static_cast<char>(status);

        QCanBusFrame frame(0x101, data);

        if (!m_device->writeFrame(frame))
        {
            std::cerr << "Failed to send 0x101\n";
        }
    }

    void printScenario()
    {
        std::cout << "SCENARIO STEPS (Loop " << m_loopCounter + 1 << "):\n";
        std::cout << " 0-2s:    Ignition OFF\n";
        std::cout << " 2-4s:    Ignition ON\n";
        std::cout << "          HIGH BEAM ON (once, stays ON forever)\n";
        std::cout << "          Left/Right turn signals blink for bulb check\n";
        std::cout << "          Check Engine ON (solid)\n";
        std::cout << " 4-19s:   Fuel filling 50% → 100%\n";
        std::cout << " 19-29s:  Engine warmup 30°C → 50°C\n";
        std::cout << " 29-33s:  Revving to 6000 RPM\n";
        std::cout << " 33-41s:  Acceleration to 100 km/h\n";
        std::cout << " 41-56s:  Smooth cruising\n";
        std::cout << " 56-70s:  EMERGENCY MODE\n";
        std::cout << " 70-72s:  Ignition OFF, HIGH BEAM OFF\n";
        std::cout << " 72-75s:  CAN LOST (3 seconds)\n";
        std::cout << "========================================\n\n";
    }

private:
    QCanBusDevice *m_device = nullptr;
    QTimer m_timer;
    std::mt19937 m_rng;

    int m_scenarioStep =0;
    int m_stepTimer = 0;
    int m_loopCounter = 0;
    int m_frameCounter = 0;

    bool m_ignitionOn = false;
    bool m_highBeamOn = false;
    double m_speed = 0;
    int m_rpm = 0;
    int m_fuel = 50;
    int m_coolant = 30;
    bool m_leftTurn = false;
    bool m_rightTurn = false;
    bool m_highBeam = false;
    bool m_checkEngine = false;
    bool m_canLost = false;
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CanGenerator generator;

    return app.exec();
}