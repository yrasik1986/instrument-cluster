#pragma once

#include <QObject>
#include <QCanBusDevice>

class CanReceiver : public QObject
{
    Q_OBJECT

public:
    explicit CanReceiver(QObject *parent = nullptr);

    bool connectToInterface(const QString &interfaceName);
    void disconnectFromInterface();

    bool isConnected() const;

signals:
    void frameReceived(const QCanBusFrame &frame);
    void connectionError(const QString &message);
    void connected();
    void disconnected();

private slots:
    void processFrames();
    void processError(QCanBusDevice::CanBusError error);

private:
    QCanBusDevice *m_device = nullptr;
};