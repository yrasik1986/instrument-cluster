#include "CanReceiver.h"

#include <QCanBus>
#include <QDebug>

CanReceiver::CanReceiver(QObject *parent)
    : QObject(parent)
{
}

bool CanReceiver::connectToInterface(
    const QString &interfaceName)
{
    if (m_device)
    {
        disconnectFromInterface();
    }

    QString errorString;

    m_device =
        QCanBus::instance()->createDevice(
            QStringLiteral("socketcan"),
            interfaceName,
            &errorString
            );

    if (!m_device)
    {
        emit connectionError(
            QStringLiteral("CAN device creation failed: %1")
                .arg(errorString)
            );

        return false;
    }

    connect(
        m_device,
        &QCanBusDevice::framesReceived,
        this,
        &CanReceiver::processFrames
        );

    connect(
        m_device,
        &QCanBusDevice::errorOccurred,
        this,
        &CanReceiver::processError
        );

    if (!m_device->connectDevice())
    {
        emit connectionError(
            QStringLiteral("CAN connection failed: %1")
                .arg(m_device->errorString())
            );

        m_device->deleteLater();
        m_device = nullptr;

        return false;
    }

    emit connected();

    return true;
}

void CanReceiver::disconnectFromInterface()
{
    if (!m_device)
        return;

    if (m_device->state() !=
        QCanBusDevice::UnconnectedState)
    {
        m_device->disconnectDevice();
    }

    m_device->deleteLater();
    m_device = nullptr;

    emit disconnected();
}

bool CanReceiver::isConnected() const
{
    return m_device &&
           m_device->state() ==
               QCanBusDevice::ConnectedState;
}

void CanReceiver::processFrames()
{
    if (!m_device)
        return;

    while (m_device->framesAvailable() > 0)
    {
        const QCanBusFrame frame =
            m_device->readFrame();

        emit frameReceived(frame);
    }
}

void CanReceiver::processError(
    QCanBusDevice::CanBusError error)
{
    Q_UNUSED(error);

    if (!m_device)
        return;

    emit connectionError(
        m_device->errorString()
        );
}