#include "model.h"

Model::Model()
    : m_discovery(NULL)
    , m_control(NULL)
    , m_service(NULL)
    , m_time()
    , m_cpu_usage()
    , m_cmd_input()
    , m_cmd_ack()
    , m_msg()
    , m_imu()
    , m_encoder()
{
    m_discovery = new QBluetoothDeviceDiscoveryAgent();
    m_discovery->setLowEnergyDiscoveryTimeout(15000);

    connect(m_discovery, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &Model::deviceScanned);
    connect(m_discovery, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &Model::scanFinished);

    qInfo("[BLE] scanStarted");
    m_discovery->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    emit connStatusUpdated(ConnStatus::scanning);
}

void Model::scanFinished() {
    qInfo("[BLE] scanFinished");
    if (m_control == NULL) {
        qInfo("[BLE] scanStarted");
        m_discovery->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    }
}

void Model::deviceScanned(const QBluetoothDeviceInfo &device) {
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        if (device.name().toLower() == "balance-car") {
            qInfo() << "[BLE] addDevice " << device.address().toString() << device.name() << device.deviceUuid() << device.deviceUuid();

            m_discovery->stop();

            m_control = QLowEnergyController::createCentral(device);

            connect(m_control, &QLowEnergyController::connected,
                    this, &Model::deviceConnected);
            connect(m_control, &QLowEnergyController::serviceDiscovered,
                    this, &Model::serviceDiscovered);
            connect(m_control, &QLowEnergyController::disconnected,
                    this, &Model::deviceDisconnected);

            emit connStatusUpdated(ConnStatus::disconnected);
        }
    }
}

void Model::connectDevice() {
    if (m_control != NULL) {
        if (m_control->state() == QLowEnergyController::UnconnectedState) {
            qInfo() << "[BLE] connectToDevice";
            m_control->connectToDevice();
        } else {
            qInfo() << "[BLE] disconnectFromDevice";
            m_control->disconnectFromDevice();
        }
    }
}

void Model::deviceConnected() {
    qInfo() << "[BLE] deviceConnected";
    m_control->discoverServices();

    emit connStatusUpdated(ConnStatus::connected);
}

void Model::deviceDisconnected() {
    qInfo() << "[BLE] deviceDisconnected";
    emit connStatusUpdated(ConnStatus::disconnected);
}

void Model::serviceDiscovered(const QBluetoothUuid &newService) {
    qInfo() << "[BLE] service " << newService;
    m_service = m_control->createServiceObject(newService);

    connect(m_service, &QLowEnergyService::stateChanged, this, &Model::serviceStateChanged);
    connect(m_service, &QLowEnergyService::characteristicChanged, this, &Model::characteristicChanged);
    m_service->discoverDetails();
}

void Model::serviceStateChanged(QLowEnergyService::ServiceState newState) {
    qInfo() << "[BLE] serviceStateChanged " << newState;

    switch (newState) {
    case QLowEnergyService::LocalService:
        qErrnoWarning("[BLE] QLowEnergyService::LocalService should not happen!");
        return;
    case QLowEnergyService::InvalidService:
    case QLowEnergyService::RemoteService:
    case QLowEnergyService::RemoteServiceDiscovering:
        return;
    case QLowEnergyService::RemoteServiceDiscovered:
        Q_FOREACH(QLowEnergyCharacteristic charac, m_service->characteristics()) {
            // qInfo() << "[BLE] charac " << charac.name() << charac.value() << charac.properties() << charac.uuid();
            auto descriptor_name = charac.descriptor(QBluetoothUuid::DescriptorType::CharacteristicUserDescription);
            if (descriptor_name.isValid()) {
                qInfo() << "[BLE] Found " << descriptor_name.value();
                if (descriptor_name.value() == QString::fromUtf8("Clock")) {
                    m_time = charac;
                } else if (descriptor_name.value() == QString::fromUtf8("CPU Usage")) {
                    m_cpu_usage = charac;
                } else if (descriptor_name.value() == QString::fromUtf8("Cmd Input")) {
                    m_cmd_input = charac;
                } else if (descriptor_name.value() == QString::fromUtf8("Cmd Ack")) {
                    m_cmd_ack = charac;
                } else if (descriptor_name.value() == QString::fromUtf8("Msg")) {
                    m_msg = charac;
                } else if (descriptor_name.value() == QString::fromUtf8("Imu")) {
                    m_imu = charac;
                } else if (descriptor_name.value() == QString::fromUtf8("Encoder")) {
                    m_encoder = charac;
                }
            }

            auto descriptor_cccd = charac.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
            if (descriptor_cccd.isValid()) {
                if (charac.properties() & QLowEnergyCharacteristic::Notify) {
                    qInfo() << "[BLE] Start Notify " << descriptor_name.value();
                    m_service->writeDescriptor(descriptor_cccd, QLowEnergyCharacteristic::CCCDEnableNotification);
                } else if (charac.properties() & QLowEnergyCharacteristic::Indicate) {
                    qInfo() << "[BLE] Start Indicate " << descriptor_name.value();
                    m_service->writeDescriptor(descriptor_cccd, QLowEnergyCharacteristic::CCCDEnableIndication);
                }
            }
        }
        return;
    }
}

void Model::characteristicChanged(const QLowEnergyCharacteristic &charac, const QByteArray &newValue) {
    auto len = newValue.length();
    // qInfo() << "[BLE] characteristicChanged " << charac.uuid() << newValue;

    QDataStream data(newValue);
    data.setByteOrder(QDataStream::LittleEndian);

    if (charac.uuid() == m_time.uuid()) {
        quint64 tick;
        data >> tick;
        emit timeUpdated(tick);

    } else if (charac.uuid() == m_cpu_usage.uuid()) {
        quint64 tick, perc;
        data >> tick >> perc;
        emit cpuUsageUpdated(tick, 100 - ((double) perc) / 100.0);

    } else if (charac.uuid() == m_cmd_ack.uuid()) {        
        quint64 id, tick;
        data >> id;
        data >> tick;
        if (id == 0 && tick == 0) {
            // Skip this
        } else {
            emit cmdAck(id, tick);
        }

    } else if (charac.uuid() == m_msg.uuid()) {
        quint64 tick;
        data >> tick;

        QString msg = QString::fromUtf8(newValue.mid(8));

        qInfo() << "[BLE] MSG " << tick << " " << msg << len;

        emit msgReceived(tick, msg);

    } else if (charac.uuid() == m_imu.uuid()) {
        quint64 tick;
        data >> tick;

        Imu imu;
        data >> imu.yaw >> imu.pitch >> imu.roll >> imu.x_accel >> imu.y_accel >> imu.z_accel;

        emit imuUpdated(tick, imu);

    } else if (charac.uuid() == m_encoder.uuid()) {
        quint64 tick;
        qreal left_rps, right_rps;
        data >> tick >> left_rps >> right_rps;

        emit encoderUpdated(tick, left_rps, right_rps);

    }
}

void Model::cmdSend(quint64 id, const QString & msg) {
    auto bytes = QByteArray();

    id = qToLittleEndian(id);
    bytes.append(reinterpret_cast<const char*>(&id), sizeof(id));
    bytes.append(msg.toUtf8());

    m_service->writeCharacteristic(m_cmd_input, bytes);
}

