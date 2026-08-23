#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "can/CanReceiver.h"
#include "model/VehicleModel.h"
#include "logic/DashboardController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    CanReceiver canReceiver;
    VehicleModel vehicleModel;

    DashboardController controller( &canReceiver, &vehicleModel);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(QStringLiteral("vehicle"), &vehicleModel);

    const QUrl url( QStringLiteral("qrc:/InstrumentCluster/qml/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,&app,[]()
        {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
        );

    engine.load(url);

    if (engine.rootObjects().isEmpty())
    {
         return -1;
    }

    QString interfaceName = QStringLiteral("vcan0");

    if (argc > 1)
    {
        interfaceName = QString::fromLocal8Bit(argv[1]);
    }

    controller.start(interfaceName);

    return app.exec();
}