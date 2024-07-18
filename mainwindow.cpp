#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QVBoxLayout>
#include <QChart>
#include <QChartView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(NULL)
    , msgsModel(new QStandardItemModel(0, 2))
    , yaw_chart(new LineChart(1000, {-180, 180}))
    , pitch_chart(new LineChart(1000, {-180, 180}))
    , roll_chart(new LineChart(1000, {-180, 180}))
    , x_accel_chart(new LineChart(1000, {-20, 20}))
    , y_accel_chart(new LineChart(1000, {-20, 20}))
    , z_accel_chart(new LineChart(1000, {-20, 20}))
    , encoder_chart(new LineChartMulti(1000, {-3, 3}, 2))
    , cpu_chart(new LineChart(1000, {0, 10}))
{
    ui->setupUi(this);

    ui->tableView_msgs->setModel(msgsModel);
    ui->tableView_msgs->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_msgs->setColumnWidth(0, 65);

    QGridLayout *gridLayout = new QGridLayout();
    ui->group_imu->setLayout(gridLayout);
    gridLayout->addWidget(cpu_chart->view, 0, 0);
    gridLayout->addWidget(encoder_chart->view, 0, 1);
    gridLayout->addWidget(yaw_chart->view, 1, 0);
    gridLayout->addWidget(pitch_chart->view, 2, 0);
    gridLayout->addWidget(roll_chart->view, 3, 0);
    gridLayout->addWidget(x_accel_chart->view, 1, 1);
    gridLayout->addWidget(y_accel_chart->view, 2, 1);
    gridLayout->addWidget(z_accel_chart->view, 3, 1);

    model = new Model();

    connect(model, &Model::timeUpdated, this, &MainWindow::timeUpdated);
    connect(model, &Model::cpuUsageUpdated, this, &MainWindow::cpuUsageUpdated);
    connect(model, &Model::connStatusUpdated, this, &MainWindow::connStatusUpdated);
    connect(model, &Model::msgReceived, this, &MainWindow::msgReceived);
    connect(ui->lineedit_msgs, &QLineEdit::returnPressed, this, &MainWindow::cmdSendStart);
    connect(model, &Model::cmdAck, this, &MainWindow::cmdSendFinish);
    connect(model, &Model::imuUpdated, this, &MainWindow::imuUpdated);
    connect(model, &Model::encoderUpdated, this, &MainWindow::encoderUpdated);

    connect(ui->btn_connect, &QPushButton::released, model, &Model::connectDevice);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timeUpdated(quint64 ticks) {
    quint32 msec = ticks % 100;
    quint32 sec = (ticks / 100) % 60;
    quint32 min = (ticks / 100) / 60;
    auto output = QString("Time = %1:%2.%3")
                      .arg(min, 2, 10, QLatin1Char('0'))
                      .arg(sec, 2, 10, QLatin1Char('0'))
                      .arg(msec, 2, 10, QLatin1Char('0'));
    ui->label_time->setText(output);
}

void MainWindow::cpuUsageUpdated(quint64 tick, double usage_perc) {
    cpu_chart->append(tick, usage_perc);
}

void MainWindow::connStatusUpdated(ConnStatus conn_status) {
    switch (conn_status) {
    case ConnStatus::connected:
        ui->btn_connect->setText("Disconnect");
        break;
    case ConnStatus::disconnected:
        ui->btn_connect->setText("Connect");
        break;
    case ConnStatus::scanning:
        ui->btn_connect->setText("Scanning");
    default:
        break;
    }
}

void MainWindow::msgReceived(quint64 tick, const QString &msg) {
    quint32 msec = tick % 100;
    quint32 sec = (tick / 100) % 60;
    quint32 min = (tick / 100) / 60;
    auto output = QString("%1:%2.%3")
                      .arg(min, 2, 10, QLatin1Char('0'))
                      .arg(sec, 2, 10, QLatin1Char('0'))
                      .arg(msec, 2, 10, QLatin1Char('0'));
    msgsModel->appendRow(QList {new QStandardItem(output), new QStandardItem(msg)});
}

void MainWindow::cmdSendStart() {
    auto msg = ui->lineedit_msgs->text();

    model->cmdSend(msgsModel->rowCount(), msg);

    ui->lineedit_msgs->clear();
    msgsModel->appendRow({new QStandardItem(""), new QStandardItem(msg)});
}

void MainWindow::cmdSendFinish(quint64 id, quint64 tick) {
    quint32 msec = tick % 100;
    quint32 sec = (tick / 100) % 60;
    quint32 min = (tick / 100) / 60;
    auto output = QString("%1:%2.%3")
                      .arg(min, 2, 10, QLatin1Char('0'))
                      .arg(sec, 2, 10, QLatin1Char('0'))
                      .arg(msec, 2, 10, QLatin1Char('0'));
    msgsModel->setItem(id, 0, new QStandardItem(output));
}

void MainWindow::imuUpdated(quint64 tick, const Imu & imu) {
    yaw_chart->append(tick, ((qreal) imu.yaw) / 100);
    pitch_chart->append(tick, ((qreal) imu.pitch) / 100);
    roll_chart->append(tick, ((qreal) imu.roll) / 100);
    x_accel_chart->append(tick, ((qreal) imu.x_accel) / 100);
    y_accel_chart->append(tick, ((qreal) imu.y_accel) / 100);
    z_accel_chart->append(tick, ((qreal) imu.z_accel) / 100);
}

void MainWindow::encoderUpdated(quint64 tick, qreal left_rps, qreal right_rps) {
    encoder_chart->append(0, tick, left_rps);
    encoder_chart->append(1, tick, right_rps);
}
