#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyService>

#include "imu.h"

enum class ConnStatus
{
    scanning,
    disconnected,
    connected,
};

class Model : public QObject
{
    Q_OBJECT
public:
    Model();

private:
    QBluetoothDeviceDiscoveryAgent *m_discovery;
    QLowEnergyController *m_control;
    QLowEnergyService *m_service;

    QLowEnergyCharacteristic m_time;
    QLowEnergyCharacteristic m_cpu_usage;
    QLowEnergyCharacteristic m_cmd_input;
    QLowEnergyCharacteristic m_cmd_ack;
    QLowEnergyCharacteristic m_msg;
    QLowEnergyCharacteristic m_imu;
    QLowEnergyCharacteristic m_encoder;

public slots:
    void scanFinished();
    void deviceScanned(const QBluetoothDeviceInfo &device);
    void deviceConnected();
    void deviceDisconnected();
    void serviceDiscovered(const QBluetoothUuid &newService);
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);

    void connectDevice();
    void cmdSend(quint64 id, const QString & msg);

signals:
    void connStatusUpdated(ConnStatus conn_status);
    void timeUpdated(quint64 tick);
    void cpuUsageUpdated(quint64 tick, double cpu_usage);
    void msgReceived(quint64 tick, const QString & msg);
    void cmdAck(quint64 id, quint64 tick);
    void imuUpdated(quint64 tick, const Imu & imu);
    void encoderUpdated(quint64 tick, qreal left_rps, qreal right_rps);
};

#endif // MODEL_H
