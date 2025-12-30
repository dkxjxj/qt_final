#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"
#include <QVBoxLayout>
#include <QDebug>

StatisticsDialog::StatisticsDialog(QWidget *parent, Database *db)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , database(db)
    , subjectChart(nullptr)
    , subjectChartView(nullptr)
    , classChart(nullptr)
    , classChartView(nullptr)
    , distributionChart(nullptr)
    , distributionChartView(nullptr)
    , trendChart(nullptr)
    , trendChartView(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("成绩统计分析");
    setMinimumSize(1000, 700);

    // 设置科目下拉框
    ui->subjectCombo->addItems({"语文", "数学", "英语"});

    // 初始化图表
    setupCharts();

    // 连接信号槽
    connect(ui->subjectCombo, &QComboBox::currentIndexChanged,
            this, &StatisticsDialog::on_subjectCombo_currentIndexChanged);

    connect(ui->classList, &QListWidget::currentTextChanged,
            this, &StatisticsDialog::on_classList_currentTextChanged);

    // 初始化数据和图表
    updateClassList();
    updateCharts();
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::setupCharts()
{
    // 科目统计图表
    subjectChart = new QChart();
    subjectChart->setTitle("科目统计");
    subjectChartView = new QChartView(subjectChart);
    subjectChartView->setRenderHint(QPainter::Antialiasing);

    // 班级统计图表
    classChart = new QChart();
    classChart->setTitle("班级对比");
    classChartView = new QChartView(classChart);
    classChartView->setRenderHint(QPainter::Antialiasing);

    // 分布图表
    distributionChart = new QChart();
    distributionChart->setTitle("成绩分布");
    distributionChartView = new QChartView(distributionChart);
    distributionChartView->setRenderHint(QPainter::Antialiasing);

    // 趋势图表
    trendChart = new QChart();
    trendChart->setTitle("成绩趋势");
    trendChartView = new QChartView(trendChart);
    trendChartView->setRenderHint(QPainter::Antialiasing);

    // 将图表添加到布局
    QVBoxLayout *layout1 = new QVBoxLayout(ui->subjectGroup);
    layout1->addWidget(subjectChartView);

    QVBoxLayout *layout2 = new QVBoxLayout(ui->classGroup);
    layout2->addWidget(classChartView);

    QVBoxLayout *layout3 = new QVBoxLayout(ui->distributionGroup);
    layout3->addWidget(distributionChartView);

    QVBoxLayout *layout4 = new QVBoxLayout(ui->trendGroup);
    layout4->addWidget(trendChartView);
}

void StatisticsDialog::updateCharts()
{
    updateSubjectStats();
    updateClassStats();
    updateDistributionChart();
    updateTrendChart();
    updateStatsPanel();
}

void StatisticsDialog::on_subjectCombo_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateCharts();
}

void StatisticsDialog::on_classList_currentTextChanged(const QString &currentText)
{
    updateStatsPanel(currentText);
}

void StatisticsDialog::updateSubjectStats()
{
    subjectChart->removeAllSeries();

    QString subject = ui->subjectCombo->currentText();
    QVector<QMap<QString, QVariant>> stats = database->getSubjectStats(subject);

    if (stats.isEmpty()) return;

    QBarSeries *series = new QBarSeries();

    QStringList categories;
    QBarSet *avgSet = new QBarSet("平均分");
    QBarSet *passSet = new QBarSet("及格率");

    for (const auto &stat : stats) {
        categories << stat["class"].toString();
        avgSet->append(stat["avg_score"].toDouble());
        passSet->append(stat["pass_rate"].toDouble());
    }

    series->append(avgSet);
    series->append(passSet);
    subjectChart->addSeries(series);

    // 设置X轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("班级");
    subjectChart->setAxisX(axisX, series);

    // 设置Y轴
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("分数/百分比");
    axisY->setRange(0, 100);
    subjectChart->setAxisY(axisY, series);

    subjectChart->setTitle(QString("%1成绩统计").arg(subject));
    subjectChart->legend()->setVisible(true);
}

void StatisticsDialog::updateClassStats()
{
    classChart->removeAllSeries();

    QVector<QMap<QString, QVariant>> stats = database->getClassStats();

    if (stats.isEmpty()) return;

    QBarSeries *series = new QBarSeries();

    QStringList categories;
    QBarSet *chineseSet = new QBarSet("语文");
    QBarSet *mathSet = new QBarSet("数学");
    QBarSet *englishSet = new QBarSet("英语");

    for (const auto &stat : stats) {
        categories << stat["class"].toString();
        chineseSet->append(stat["chinese_avg"].toDouble());
        mathSet->append(stat["math_avg"].toDouble());
        englishSet->append(stat["english_avg"].toDouble());
    }

    series->append(chineseSet);
    series->append(mathSet);
    series->append(englishSet);
    classChart->addSeries(series);

    // 设置坐标轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("班级");
    classChart->setAxisX(axisX, series);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("平均分");
    axisY->setRange(0, 100);
    classChart->setAxisY(axisY, series);

    classChart->legend()->setVisible(true);
}

void StatisticsDialog::updateDistributionChart()
{
    distributionChart->removeAllSeries();

    QString subject = ui->subjectCombo->currentText();
    QVector<QMap<QString, QVariant>> distribution = database->getScoreDistribution(subject);

    if (distribution.isEmpty()) return;

    QPieSeries *series = new QPieSeries();

    for (const auto &item : distribution) {
        QString range = item["range"].toString();
        int count = item["count"].toInt();
        if (count > 0) {
            series->append(QString("%1分 (%2人)").arg(range).arg(count), count);
        }
    }

    distributionChart->addSeries(series);
    distributionChart->setTitle(QString("%1成绩分布").arg(subject));

    // 显示百分比
    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
        slice->setLabel(slice->label() + " " + QString::number(slice->percentage() * 100, 'f', 1) + "%");
    }
}

void StatisticsDialog::updateTrendChart()
{
    trendChart->removeAllSeries();

    QVector<QMap<QString, QVariant>> trendData = database->getTrendData();

    if (trendData.isEmpty()) return;

    QLineSeries *chineseSeries = new QLineSeries();
    chineseSeries->setName("语文");

    QLineSeries *mathSeries = new QLineSeries();
    mathSeries->setName("数学");

    QLineSeries *englishSeries = new QLineSeries();
    englishSeries->setName("英语");

    QStringList categories;

    for (int i = 0; i < trendData.size(); i++) {
        const auto &data = trendData[i];
        categories << data["class"].toString();

        chineseSeries->append(i, data["chinese"].toDouble());
        mathSeries->append(i, data["math"].toDouble());
        englishSeries->append(i, data["english"].toDouble());
    }

    trendChart->addSeries(chineseSeries);
    trendChart->addSeries(mathSeries);
    trendChart->addSeries(englishSeries);

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("班级");
    axisX->setRange(0, categories.size() - 1);
    trendChart->setAxisX(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("平均分");
    axisY->setRange(0, 100);
    trendChart->setAxisY(axisY);

    // 连接系列到坐标轴
    chineseSeries->attachAxis(axisX);
    chineseSeries->attachAxis(axisY);
    mathSeries->attachAxis(axisX);
    mathSeries->attachAxis(axisY);
    englishSeries->attachAxis(axisX);
    englishSeries->attachAxis(axisY);

    trendChart->legend()->setVisible(true);
}

void StatisticsDialog::updateStatsPanel(const QString &className)
{
    QString subject = ui->subjectCombo->currentText();
    QVector<QMap<QString, QVariant>> stats;

    if (className.isEmpty()) {
        // 获取全部统计数据
        QVector<QMap<QString, QVariant>> allStats = database->getSubjectStats(subject);
        if (allStats.isEmpty()) return;

        // 计算全校平均值
        double totalAvg = 0;
        double totalMax = 0;
        double totalMin = 100;
        int totalCount = 0;
        int totalPassCount = 0;

        for (const auto &stat : allStats) {
            totalAvg += stat["avg_score"].toDouble() * stat["count"].toInt();
            totalMax = qMax(totalMax, stat["max_score"].toDouble());
            totalMin = qMin(totalMin, stat["min_score"].toDouble());
            totalCount += stat["count"].toInt();
            totalPassCount += stat["pass_rate"].toDouble() * stat["count"].toInt() / 100.0;
        }

        double avgScore = totalCount > 0 ? totalAvg / totalCount : 0;
        double passRate = totalCount > 0 ? (totalPassCount * 100.0 / totalCount) : 0;

        ui->avgValue->setText(QString::number(avgScore, 'f', 1));
        ui->maxValue->setText(QString::number(totalMax, 'f', 1));
        ui->minValue->setText(QString::number(totalMin, 'f', 1));
        ui->passValue->setText(QString::number(passRate, 'f', 1) + "%");
        ui->countValue->setText(QString::number(totalCount));
    } else {
        // 获取指定班级的统计数据
        QVector<QMap<QString, QVariant>> allStats = database->getSubjectStats(subject);
        for (const auto &stat : allStats) {
            if (stat["class"].toString() == className) {
                ui->avgValue->setText(QString::number(stat["avg_score"].toDouble(), 'f', 1));
                ui->maxValue->setText(QString::number(stat["max_score"].toDouble(), 'f', 1));
                ui->minValue->setText(QString::number(stat["min_score"].toDouble(), 'f', 1));
                ui->passValue->setText(QString::number(stat["pass_rate"].toDouble(), 'f', 1) + "%");
                ui->countValue->setText(stat["count"].toString());
                break;
            }
        }
    }
}

void StatisticsDialog::updateClassList()
{
    ui->classList->clear();

    // 添加"全部"选项
    ui->classList->addItem("全部");

    // 获取所有班级
    QStringList classes = database->getAllClasses();
    for (const QString &className : classes) {
        ui->classList->addItem(className);
    }

    // 默认选中"全部"
    ui->classList->setCurrentRow(0);
}
