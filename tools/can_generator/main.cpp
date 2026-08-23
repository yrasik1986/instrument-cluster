#include <QCoreApplication>
#include <QTimer>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>

class CanGenerator : public QObject
{
    Q_OBJECT

public:
    explicit CanGenerator(QObject *parent = nullptr)
        : QObject(parent)
    {
        QString errorString;

        m_device =  QCanBus::instance()->createDevice(QStringLiteral("socketcan"), QStringLiteral("vcan0"), &errorString);

        if (!m_device)
        {
            std::cerr << "CAN device error: "<< errorString.toStdString() << '\n';
            return;
        }

        if (!m_device->connectDevice())
        {
            std::cerr << "CAN connection error: " << m_device->errorString().toStdString() << '\n';
            return;
        }

        connect( &m_timer, &QTimer::timeout,this, &CanGenerator::sendData);

        m_timer.start(20);

        std::cout   << "CAN generator started\n";
    }

private slots:

    void sendData()
    {
        m_time += 0.02;

        const double wave =  (std::sin(m_time * 0.4) + 1.0) / 2.0;

        const double speed = wave * 160.0;

        const int rpm =  static_cast<int>(1000.0 + wave * 5000.0);

        const int fuel = 50 + static_cast<int>(40.0 * std::sin(m_time * 0.05));

        const int coolant =  85 + static_cast<int>(30.0 * wave);

        send100(speed, rpm, true);

        send101(coolant,fuel);
    }

private:

    void send100(double speed,int rpm, bool ignition)
    {
        const int rawSpeed = static_cast<int>(speed * 100.0);

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

    void send101( int coolant,int fuel)
    {
        QByteArray data(8, 0);

        data[0] = static_cast<char>(std::clamp(coolant, 0, 150));

        data[1] = static_cast<char>(std::clamp(fuel, 0, 100));

        std::uint8_t status = 0;

        /*
         * Левый поворотник
         */
        if (static_cast<int>(m_time) % 8 < 2)
            status |= (1 << 0);

        /*
         * Правый поворотник
         */
        if (static_cast<int>(m_time) % 8 >= 4 &&
            static_cast<int>(m_time) % 8 < 6)
        {
            status |= (1 << 1);
        }

        /*
         * Дальний свет
         */
        if (static_cast<int>(m_time) % 20 >= 10 &&
            static_cast<int>(m_time) % 20 < 15)
        {
            status |= (1 << 2);
        }

        /*
         * Check Engine
         */
        if (coolant > 108)
        {
            status |= (1 << 3);
        }

        data[2] = static_cast<char>(status);

        QCanBusFrame frame(0x101,data);

        if (!m_device->writeFrame(frame))
        {
            std::cerr << "Failed to send 0x101\n";
        }
    }

private:

    QCanBusDevice *m_device = nullptr;

    QTimer m_timer;

    double m_time = 0.0;
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CanGenerator generator;

    return app.exec();
}