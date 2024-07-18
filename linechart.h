#ifndef LINECHART_H
#define LINECHART_H

#include <QObject>

#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>

class LineChart
{
public:
    LineChart(qreal x_bounds, QPair<qreal, qreal> y_bounds);

    QChartView *view;

private:
    QLineSeries *series;
    QChart *chart;
    QValueAxis *x_axis;
    QValueAxis *y_axis;

    qreal x_bounds;

public slots:
    void append(qreal x, qreal y);
};

#endif // LINECHART_H
