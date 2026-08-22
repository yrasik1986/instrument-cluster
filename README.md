# Vehicle Instrument Cluster

Demo instrument cluster implemented using:

- C++17
- Qt 6
- Qt Quick / QML
- CMake
- QCanBus
- SocketCAN
- Linux vcan0

## Architecture

CAN interface
    |
    v
CanReceiver
    |
    v
CanParser
    |
    v
VehicleModel
    |
    v
DashboardController
    |
    v
QML Dashboard

## Build

Install dependencies:

    sudo apt update

    sudo apt install \
        build-essential \
        cmake \
        ninja-build \
        qt6-base-dev \
        qt6-declarative-dev \
        qt6-connectivity-dev \
        qml6-module-qtquick \
        qml6-module-qtquick-controls \
        qml6-module-qtquick-layouts \
        can-utils

Create virtual CAN:

    sudo modprobe vcan

    sudo ip link add dev vcan0 type vcan

    sudo ip link set vcan0 up

Build:

    cmake -S . -B build -G Ninja

    cmake --build build

Run:

    ./build/InstrumentCluster

## CAN interface

The application uses:

    socketcan

and connects to:

    vcan0

A different interface can be specified:

    ./build/InstrumentCluster can0

## CAN ID 0x100

Period: 20 ms

Byte 0-1:

    Vehicle speed

uint16 little endian.

Physical value:

    raw * 0.01 km/h

Byte 2-3:

    Engine RPM

uint16 little endian.

Byte 4:

    Ignition

0 = OFF
1 = ON

Example:

    cansend vcan0 100#8813AC0D01

This represents:

    Speed = 50.00 km/h
    RPM = 3500
    Ignition = ON

## CAN ID 0x101

Period: 100 ms

Byte 0:

    Coolant temperature

Byte 1:

    Fuel level

Byte 2:

    Status bits

Bit 0:

    Left turn signal

Bit 1:

    Right turn signal

Bit 2:

    High beam

Bit 3:

    Check Engine

## Manual tests

### 50 km/h / 3500 RPM / ignition ON

    cansend vcan0 100#8813AC0D01

### Coolant 95°C / fuel 100%

    cansend vcan0 101#5F6400

### Low fuel

    cansend vcan0 101#5A0800

### High temperature

    cansend vcan0 101#6A6400

### Check Engine

    cansend vcan0 101#5A6408

### Left turn + high beam

    cansend vcan0 101#5A6405

## Monitor CAN traffic

    candump vcan0

## CAN generator

A test CAN generator is provided.

Run:

    ./build/tools/can_generator/can_generator

The generator continuously sends:

    0x100 every 20 ms
    0x101 every 20 ms

The generated values change dynamically.

## CAN timeout

If CAN ID 0x100 is not received for more than 1000 ms:

- CAN LOST indicator is displayed;
- previous speed and RPM values are invalidated;
- turn indicators are cleared;
- high beam and Check Engine are cleared.

When CAN communication resumes, the dashboard automatically returns to normal operation.

## Warnings

Low fuel:

    fuel < 10%

High coolant temperature:

    coolant > 105°C

## Smooth gauges

Gauge needles are animated in QML using NumberAnimation.

Incoming CAN data is processed in C++ and does not block the graphical interface.

## Project structure

src/
    can/
        CanReceiver
        CanParser

    model/
        VehicleData
        VehicleModel

    logic/
        DashboardController

qml/
    Dashboard
    Speedometer
    Tachometer
    Indicator

tools/
    can_generator/
