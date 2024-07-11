#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QLowEnergyController>
#include <QStandardItemModel>

#include "model.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Model *model;
    QStandardItemModel *msgsModel;

public slots:
    // For model
    void timeUpdated(quint64 ticks);
    void cpuUsageUpdated(double usage_perc);
    void connStatusUpdated(ConnStatus conn_status);
    void msgReceived(quint64 tick, const QString &str);
    void cmdSendStart();
    void cmdSendFinish(quint64 tick);
    // For controller
};
#endif // MAINWINDOW_H
