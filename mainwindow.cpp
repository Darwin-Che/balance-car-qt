#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QAbstractButton::clicked, this, &MainWindow::ble);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ble(bool clicked)
{
    qInfo("ble clicked");
    QBluetoothDeviceDiscoveryAgent * discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(15000);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &MainWindow::addDevice);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &MainWindow::scanFinished);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &MainWindow::scanFinished);
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void MainWindow::scanFinished() {
    qInfo("[ble] scanFinished");
}

void MainWindow::addDevice(const QBluetoothDeviceInfo &device) {
    qInfo() << "[ble] addDevice " << device.address().toString() << device.name();
}
