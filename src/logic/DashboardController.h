#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>

#include "../can/CanParser.h"
#include "../can/CanReceiver.h"
#include "../model/VehicleModel.h"

/**
 * @brief Контроллер приборной панели
 *
 * Обеспечивает связь между CAN-шиной и моделью данных автомобиля.
 * Принимает CAN-кадры, парсит их и обновляет модель.
 * Контролирует таймаут соединения с CAN-шиной.
 *
 * Основные функции:
 * - Прием CAN-кадров через CanReceiver
 * - Парсинг данных через CanParser
 * - Обновление VehicleModel
 * - Мониторинг потери связи с CAN (таймаут 1 секунда)
 */

class DashboardController : public QObject
{
    Q_OBJECT

public:
    explicit DashboardController(CanReceiver *receiver,VehicleModel *model, QObject *parent = nullptr);

    bool start(const QString &interfaceName);

private slots:
    void processFrame(const QCanBusFrame &frame);
    void checkCanTimeout();

private:
    CanReceiver *m_receiver;
    VehicleModel *m_model;

    CanParser m_parser;

    QTimer m_timeoutTimer;
    QElapsedTimer m_lastCan100;

    bool m_receivedCan100 = false;

    static constexpr qint64 CAN_TIMEOUT_MS = 1000;
};