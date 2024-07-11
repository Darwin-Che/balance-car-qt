#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothDeviceInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(NULL)
    , msgsModel(new QStandardItemModel(0, 2))
{
    ui->setupUi(this);

    ui->tableView_msgs->setModel(msgsModel);
    ui->tableView_msgs->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_msgs->setColumnWidth(0, 50);

    model = new Model();

    connect(model, &Model::timeUpdated, this, &MainWindow::timeUpdated);
    connect(model, &Model::cpuUsageUpdated, this, &MainWindow::cpuUsageUpdated);
    connect(model, &Model::connStatusUpdated, this, &MainWindow::connStatusUpdated);
    connect(model, &Model::msgReceived, this, &MainWindow::msgReceived);
    connect(ui->lineedit_msgs, &QLineEdit::returnPressed, this, &MainWindow::cmdSendStart);
    connect(model, &Model::cmdSendFinish, this, &MainWindow::cmdSendFinish);

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
    emit ui->label_time->setText(output);
}

void MainWindow::cpuUsageUpdated(double usage_perc) {
    auto output = QString("CPU Usage = %1%")
                      .arg(usage_perc);
    emit ui->label_cpu_usage->setText(output);
}

void MainWindow::connStatusUpdated(ConnStatus conn_status) {
    switch (conn_status) {
    case ConnStatus::connected:
        emit ui->btn_connect->setText("Disconnect");
        break;
    case ConnStatus::disconnected:
        emit ui->btn_connect->setText("Connect");
        break;
    case ConnStatus::scanning:
        emit ui->btn_connect->setText("Scanning");
    default:
        break;
    }
}

void MainWindow::msgReceived(quint64 tick, const QString &msg) {
    msgsModel->appendRow(QList {new QStandardItem(QString::number(tick)), new QStandardItem(msg)});
}

void MainWindow::cmdSendStart() {
    auto msg = ui->lineedit_msgs->text();
    emit model->cmdSend(msg);

    ui->lineedit_msgs->clear();
    msgsModel->appendRow({new QStandardItem(""), new QStandardItem(msg)});
}

void MainWindow::cmdSendFinish(quint64 tick) {
    // the last command should be within the last 32 logs
    auto l = msgsModel->rowCount();
    for (int row = 0; row < 16; row += 1) {
        auto r = l - 1 - row;
        if (msgsModel->item(r, 0)->data(Qt::DisplayRole) == "") {
            msgsModel->setItem(r, 0, new QStandardItem(QString::number(tick)));
            return;
        }
    }
}
