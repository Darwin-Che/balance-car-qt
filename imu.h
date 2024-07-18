#ifndef IMU_H
#define IMU_H

#include <stdint.h>

#include <QObject>
#include <QGridLayout>
#include <QtCharts/QLineSeries>
#include <QChartView>
#include <QValueAxis>

struct Imu {
    int16_t yaw;
    int16_t pitch;
    int16_t roll;
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
};

// class ImuDisplay : public QObject
// {
//     Q_OBJECT
// public:
//     ImuDisplay();

// private:
//     LineChart *yaw_chart;

// public slots:


// signals:
// };

#endif // IMU_H
