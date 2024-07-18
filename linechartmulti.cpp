#include "linechartmulti.h"

LineChartMulti::LineChartMulti(qreal x_bounds, QPair<qreal, qreal> y_bounds, int numSeries)
    : seriesList()
    , chart(new QChart())
    , view(NULL)
    , x_axis(new QValueAxis())
    , y_axis(new QValueAxis())
    , x_bounds(x_bounds)
{
    for (int i = 0; i < numSeries; i += 1) {
        auto s = new QLineSeries();
        seriesList.append(s);
        chart->addSeries(s);
    }

    chart->legend()->hide();
    chart->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);

    x_axis->setRange(0, x_bounds);
    y_axis->setRange(y_bounds.first, y_bounds.second);

    chart->addAxis(x_axis, Qt::AlignBottom);
    chart->addAxis(y_axis, Qt::AlignRight);

    for (int i = 0; i < numSeries; i += 1) {
        seriesList[i]->attachAxis(x_axis);
        seriesList[i]->attachAxis(y_axis);
    }

    view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
}

void LineChartMulti::append(int seriesId, qreal x, qreal y) {
    seriesList[seriesId]->append(x, y);

    x_axis->setMax(qMax(x_axis->max(), x));
    x_axis->setMin(x_axis->max() - x_bounds);
    // y_axis->setMax(qMax(y_axis->max(), y));
}
