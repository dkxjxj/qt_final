#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "database.h"

// Qt 6中Charts模块的包含方式
#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QLineSeries>
#include <QPieSeries>
#include <QValueAxis>
#include <QBarCategoryAxis>

// 在Qt 6中，不需要 using namespace
// 也不需要 QT_CHARTS_USE_NAMESPACE

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget *parent = nullptr, Database *db = nullptr);
    ~StatisticsDialog();

private slots:
    void updateCharts();
    void on_subjectCombo_currentIndexChanged(int index);
    void on_classList_currentTextChanged(const QString &currentText);

private:
    void setupCharts();
    void updateSubjectStats();
    void updateClassStats();
    void updateDistributionChart();
    void updateTrendChart();
    void updateStatsPanel(const QString &className = "");
    void updateClassList();

    Ui::StatisticsDialog *ui;
    Database *database;

    // 直接使用类型，不需要前缀
    QChart *subjectChart;
    QChartView *subjectChartView;

    QChart *classChart;
    QChartView *classChartView;

    QChart *distributionChart;
    QChartView *distributionChartView;

    QChart *trendChart;
    QChartView *trendChartView;
};

#endif // STATISTICSDIALOG_H
