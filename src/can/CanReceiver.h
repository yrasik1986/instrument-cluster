#pragma once

#include <QObject>
#include <QCanBusDevice>

/**
 * @brief Приемник CAN-кадров
 *
 * Обеспечивает подключение к CAN-интерфейсу и прием кадров.
 * Работает через Qt CAN Bus API (socketcan на Linux).
 *
 * Основные функции:
 * - Подключение к CAN-интерфейсу (connectToInterface)
 * - Отключение от интерфейса (disconnectFromInterface)
 * - Прием CAN-кадров и их передача через сигнал frameReceived
 * - Обработка ошибок соединения
 *
 * @note Использует QCanBusDevice для работы с CAN
 * @note Поддерживается только socketcan (Linux)
 */

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