#include "linechart.h"

LineChart::LineChart(qreal x_bounds, QPair<qreal, qreal> y_bounds)
    : series(new QLineSeries())
    , chart(new QChart())
    , view(NULL)
    , x_axis(new QValueAxis())
    , y_axis(new QValueAxis())
    , x_bounds(x_bounds)
{
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);

    x_axis->setRange(0, x_bounds);
    y_axis->setRange(y_bounds.first, y_bounds.second);

    chart->addAxis(x_axis, Qt::AlignBottom);
    chart->addAxis(y_axis, Qt::AlignRight);

    series->attachAxis(x_axis);
    series->attachAxis(y_axis);

    view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
}

void LineChart::append(qreal x, qreal y) {
    series->append(x, y);

    x_axis->setMax(qMax(x_axis->max(), x));
    x_axis->setMin(x_axis->max() - x_bounds);
    // y_axis->setMax(qMax(y_axis->max(), y));
}
