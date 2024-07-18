#ifndef LINECHARTMULTI_H
#define LINECHARTMULTI_H

#include <QObject>

#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>

class LineChartMulti
{
public:
    LineChartMulti(qreal x_bounds, QPair<qreal, qreal> y_bounds, int numSeries);

    QChartView *view;

private:
    QList<QLineSeries *> seriesList;
    QChart *chart;
    QValueAxis *x_axis;
    QValueAxis *y_axis;

    qreal x_bounds;

public slots:
    void append(int seriesId, qreal x, qreal y);
};

#endif // LINECHARTMULTI_H
