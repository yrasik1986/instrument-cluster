cmake_minimum_required(VERSION 3.16)

project(InstrumentCluster
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    SerialBus
)

qt_add_executable(InstrumentClusterApp
    src/main.cpp

    src/can/CanReceiver.cpp
    src/can/CanReceiver.h

    src/can/CanParser.cpp
    src/can/CanParser.h

    src/model/VehicleModel.cpp
    src/model/VehicleModel.h
    src/model/VehicleData.h

    src/logic/DashboardController.cpp
    src/logic/DashboardController.h
)

qt_add_qml_module(InstrumentClusterApp
    URI InstrumentCluster
    VERSION 1.0

    QML_FILES
        qml/Main.qml
        qml/Dashboard.qml
        qml/Speedometer.qml
        qml/Tachometer.qml
        qml/Indicator.qml
)

target_link_libraries(InstrumentClusterApp
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::SerialBus
)

target_include_directories(InstrumentClusterApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_subdirectory(tools/can_generator)
