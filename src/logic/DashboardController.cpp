#include "DashboardController.h"

#include "../can/CanFrameId.h"

#include <QDebug>

DashboardController::DashboardController(CanReceiver *receiver, VehicleModel *model, QObject *parent)
    : QObject(parent)
    , m_receiver(receiver)
    , m_model(model)
{
    connect(m_receiver,&CanReceiver::frameReceived,this,&DashboardController::processFrame);

    connect(&m_timeoutTimer,&QTimer::timeout, this, &DashboardController::checkCanTimeout);

    m_timeoutTimer.setInterval(100);
}

bool DashboardController::start(const QString &interfaceName)
{
    m_timeoutTimer.start();

    return m_receiver->connectToInterface(interfaceName);
}

void DashboardController::processFrame(const QCanBusFrame &frame)
{
    VehicleData data;

    if (!m_parser.parse(frame, data))
    {
        return;
    }

    switch (frame.frameId())
    {
    case static_cast<std::uint32_t>(VehicleCan::FrameId::DriveData):
        m_model->updateDriveData(data.speed, data.rpm,data.ignition);
        restartLastCan();
        break;
    case static_cast<std::uint32_t>(VehicleCan::FrameId::StatusData):
        m_model->updateStatusData(data.coolantTemperature, data.fuel,data.leftTurn,data.rightTurn, data.highBeam, data.checkEngine );
        restartLastCan();
        break;
    default:
        break;
    }
}

void DashboardController::checkCanTimeout()
{
    if (!m_receivedCan || m_lastCan.elapsed() > CAN_TIMEOUT_MS) {
        m_model->setCanLost();
    }
    m_receivedCan = false;
}

void DashboardController::restartLastCan()
{
    m_lastCan.restart();
    m_receivedCan = true;
}