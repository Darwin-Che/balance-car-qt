#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QLowEnergyController>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QtCharts/QLineSeries>
#include <QChartView>

#include "model.h"
#include "imu.h"
#include "linechart.h"
#include "linechartmulti.h"

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
    QGridLayout *grid_imu;
    LineChart *yaw_chart;
    LineChart *pitch_chart;
    LineChart *roll_chart;
    LineChart *x_accel_chart;
    LineChart *y_accel_chart;
    LineChart *z_accel_chart;
    LineChartMulti *encoder_chart;
    LineChart *cpu_chart;

public slots:
    // For model
    void timeUpdated(quint64 ticks);
    void cpuUsageUpdated(quint64 tick, double usage_perc);
    void connStatusUpdated(ConnStatus conn_status);
    void msgReceived(quint64 tick, const QString &str);
    void cmdSendStart();
    void cmdSendFinish(quint64 id, quint64 tick);
    void imuUpdated(quint64 tick, const Imu & imu);
    void encoderUpdated(quint64 tick, qreal left_rps, qreal right_rps);
    // For controller
};
#endif // MAINWINDOW_H
