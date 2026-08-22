#include "DashboardController.h"

#include <QDebug>

DashboardController::DashboardController(
    CanReceiver *receiver,
    VehicleModel *model,
    QObject *parent)
    : QObject(parent)
    , m_receiver(receiver)
    , m_model(model)
{
    connect(
        m_receiver,
        &CanReceiver::frameReceived,
        this,
        &DashboardController::processFrame
        );

    connect(
        &m_timeoutTimer,
        &QTimer::timeout,
        this,
        &DashboardController::checkCanTimeout
        );

    m_timeoutTimer.setInterval(100);
}

bool DashboardController::start(
    const QString &interfaceName)
{
    m_timeoutTimer.start();

    return m_receiver->connectToInterface(
        interfaceName
        );
}

void DashboardController::processFrame(
    const QCanBusFrame &frame)
{
    VehicleData data;

    if (!m_parser.parse(frame, data))
        return;

    if (frame.frameId() == 0x100)
    {
        m_lastCan100.restart();
        m_receivedCan100 = true;
    }

    m_model->setData(data);
}

void DashboardController::checkCanTimeout()
{
    if (!m_receivedCan100)
    {
        m_model->setCanLost();
        return;
    }

    if (m_lastCan100.elapsed() > CAN_TIMEOUT_MS)
    {
        m_model->setCanLost();
    }
}