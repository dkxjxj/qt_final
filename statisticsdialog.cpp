#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QProgressBar>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>

StatisticsDialog::StatisticsDialog(QWidget *parent, Database *db)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , database(db)
    , subjectTable(nullptr)
    , classTable(nullptr)
    , distributionWidget(nullptr)
    , trendWidget(nullptr)
{
    if (!database) {
        QMessageBox::critical(parent, "错误", "数据库连接无效！");
        return;
    }

    ui->setupUi(this);
    setWindowTitle("成绩统计分析");
    setMinimumSize(1000, 600);

    // 设置科目下拉框
    ui->subjectCombo->addItems({"语文", "数学", "英语"});

    // 延迟初始化，确保UI加载完成
    QTimer::singleShot(100, this, [this]() {
        qDebug() << "=== 开始初始化 ===";
        initializeWidgets();
        updateClassList();  // 从数据库获取班级列表
        updateAllData();    // 从数据库获取所有数据
        qDebug() << "=== 初始化完成 ===";
    });
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::initializeWidgets()
{
    qDebug() << "正在初始化控件...";

    // 1. 科目统计表格
    if (!subjectTable && ui->subjectGroup) {
        subjectTable = new QTableWidget(this);
        subjectTable->setColumnCount(4);
        subjectTable->setHorizontalHeaderLabels(QStringList() << "班级" << "平均分" << "及格率" << "人数");
        subjectTable->verticalHeader()->setVisible(false);
        subjectTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        subjectTable->setMinimumHeight(200);

        if (!ui->subjectGroup->layout()) {
            ui->subjectGroup->setLayout(new QVBoxLayout());
        }
        ui->subjectGroup->layout()->addWidget(subjectTable);
        qDebug() << "科目表格已创建";
    }

    // 2. 班级对比表格
    if (!classTable && ui->classGroup) {
        classTable = new QTableWidget(this);
        classTable->setColumnCount(5);
        classTable->setHorizontalHeaderLabels(QStringList() << "班级" << "语文" << "数学" << "英语" << "总分");
        classTable->verticalHeader()->setVisible(false);
        classTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        classTable->setMinimumHeight(200);

        if (!ui->classGroup->layout()) {
            ui->classGroup->setLayout(new QVBoxLayout());
        }
        ui->classGroup->layout()->addWidget(classTable);
        qDebug() << "班级表格已创建";
    }

    // 3. 成绩分布控件
    if (!distributionWidget && ui->distributionGroup) {
        distributionWidget = new QWidget(this);
        distributionWidget->setLayout(new QVBoxLayout());
        distributionWidget->setMinimumHeight(200);

        if (!ui->distributionGroup->layout()) {
            ui->distributionGroup->setLayout(new QVBoxLayout());
        }
        ui->distributionGroup->layout()->addWidget(distributionWidget);
        qDebug() << "分布控件已创建";
    }

    // 4. 趋势分析控件
    if (!trendWidget && ui->trendGroup) {
        trendWidget = new QWidget(this);
        trendWidget->setLayout(new QVBoxLayout());
        trendWidget->setMinimumHeight(200);

        if (!ui->trendGroup->layout()) {
            ui->trendGroup->setLayout(new QVBoxLayout());
        }
        ui->trendGroup->layout()->addWidget(trendWidget);
        qDebug() << "趋势控件已创建";
    }
}

void StatisticsDialog::updateAllData()
{
    qDebug() << "正在从数据库更新所有数据...";

    // 依次更新所有数据
    updateSubjectData();
    updateClassData();
    updateDistributionData();
    updateTrendData();
    updateStatsPanel();
}

void StatisticsDialog::updateSubjectData()
{
    qDebug() << "正在更新科目数据...";

    if (!subjectTable || !database) {
        qDebug() << "错误：表格或数据库为空";
        return;
    }

    try {
        QString subject = ui->subjectCombo->currentText();
        qDebug() << "获取科目统计：" << subject;

        QVector<QMap<QString, QVariant>> stats = database->getSubjectStats(subject);
        qDebug() << "从数据库获取到" << stats.size() << "条记录";

        subjectTable->clear();
        subjectTable->setRowCount(stats.size());
        subjectTable->setColumnCount(4);
        subjectTable->setHorizontalHeaderLabels(QStringList() << "班级" << "平均分" << "及格率" << "人数");

        if (stats.isEmpty()) {
            qDebug() << "数据库中没有数据";
            // 显示空数据提示
            subjectTable->setRowCount(1);
            subjectTable->setItem(0, 0, new QTableWidgetItem("暂无数据"));
        } else {
            for (int i = 0; i < stats.size(); i++) {
                const auto &stat = stats[i];

                // 获取班级名称并简化（去掉"年级"）
                QString className = stat["class"].toString();
                className = simplifyClassName(className);  // 简化班级名称

                QString avgScore = QString::number(stat["avg_score"].toDouble(), 'f', 1);
                QString passRate = QString::number(stat["pass_rate"].toDouble(), 'f', 1) + "%";
                QString count = stat["count"].toString();

                subjectTable->setItem(i, 0, new QTableWidgetItem(className));
                subjectTable->setItem(i, 1, new QTableWidgetItem(avgScore));
                subjectTable->setItem(i, 2, new QTableWidgetItem(passRate));
                subjectTable->setItem(i, 3, new QTableWidgetItem(count));

                qDebug() << "  班级:" << className << "平均分:" << avgScore
                         << "及格率:" << passRate << "人数:" << count;
            }
        }

        subjectTable->resizeColumnsToContents();
        qDebug() << "科目数据更新完成";

    } catch (const std::exception &e) {
        qDebug() << "更新科目数据时出错：" << e.what();
        subjectTable->setRowCount(1);
        subjectTable->setItem(0, 0, new QTableWidgetItem("数据加载失败"));
    } catch (...) {
        qDebug() << "更新科目数据时发生未知错误";
    }
}

void StatisticsDialog::updateClassData()
{
    qDebug() << "正在更新班级对比数据...";

    if (!classTable || !database) {
        qDebug() << "错误：表格或数据库为空";
        return;
    }

    try {
        QVector<QMap<QString, QVariant>> stats = database->getClassStats();
        qDebug() << "从数据库获取到" << stats.size() << "条班级记录";

        classTable->clear();
        classTable->setRowCount(stats.size());
        classTable->setColumnCount(5);
        classTable->setHorizontalHeaderLabels(QStringList() << "班级" << "语文" << "数学" << "英语" << "总分");

        if (stats.isEmpty()) {
            qDebug() << "数据库中没有班级数据";
            classTable->setRowCount(1);
            classTable->setItem(0, 0, new QTableWidgetItem("暂无数据"));
        } else {
            for (int i = 0; i < stats.size(); i++) {
                const auto &stat = stats[i];

                // 获取并简化班级名称
                QString className = stat["class"].toString();
                className = simplifyClassName(className);

                double chinese = stat["chinese_avg"].toDouble();
                double math = stat["math_avg"].toDouble();
                double english = stat["english_avg"].toDouble();
                double total = chinese + math + english;

                classTable->setItem(i, 0, new QTableWidgetItem(className));
                classTable->setItem(i, 1, new QTableWidgetItem(QString::number(chinese, 'f', 1)));
                classTable->setItem(i, 2, new QTableWidgetItem(QString::number(math, 'f', 1)));
                classTable->setItem(i, 3, new QTableWidgetItem(QString::number(english, 'f', 1)));
                classTable->setItem(i, 4, new QTableWidgetItem(QString::number(total, 'f', 1)));

                qDebug() << "  班级:" << className << "语文:" << chinese
                         << "数学:" << math << "英语:" << english << "总分:" << total;
            }
        }

        classTable->resizeColumnsToContents();
        qDebug() << "班级对比数据更新完成";

    } catch (...) {
        qDebug() << "更新班级数据时出错";
        classTable->setRowCount(1);
        classTable->setItem(0, 0, new QTableWidgetItem("数据加载失败"));
    }
}

void StatisticsDialog::updateDistributionData()
{
    qDebug() << "正在更新成绩分布数据...";

    if (!distributionWidget || !database) {
        qDebug() << "错误：控件或数据库为空";
        return;
    }

    try {
        QString subject = ui->subjectCombo->currentText();
        QVector<QMap<QString, QVariant>> distribution = database->getScoreDistribution(subject);
        qDebug() << "从数据库获取到" << distribution.size() << "条分布记录";

        // 清除旧内容
        QLayout *layout = distributionWidget->layout();
        if (layout) {
            while (QLayoutItem* item = layout->takeAt(0)) {
                if (item->widget()) {
                    item->widget()->deleteLater();
                }
                delete item;
            }
        }

        if (distribution.isEmpty()) {
            QLabel *noDataLabel = new QLabel("暂无分布数据");
            noDataLabel->setAlignment(Qt::AlignCenter);
            noDataLabel->setStyleSheet("color: gray; font-size: 14px;");
            layout->addWidget(noDataLabel);
            qDebug() << "没有分布数据";
            return;
        }

        // 计算总人数
        int totalCount = 0;
        for (const auto &item : distribution) {
            totalCount += item["count"].toInt();
        }
        qDebug() << "总人数:" << totalCount;

        // 显示分布数据
        for (const auto &item : distribution) {
            QString range = item["range"].toString();
            int count = item["count"].toInt();

            if (count <= 0) continue;

            double percentage = totalCount > 0 ? (count * 100.0 / totalCount) : 0;

            QWidget *row = new QWidget();
            QHBoxLayout *rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(5, 2, 5, 2);

            QLabel *label = new QLabel(range + "分:");
            label->setMinimumWidth(60);

            QProgressBar *bar = new QProgressBar();
            bar->setRange(0, 100);
            bar->setValue(percentage);
            bar->setFormat(QString("%1人 (%2%)").arg(count).arg(percentage, 0, 'f', 1));
            bar->setMinimumHeight(25);

            // 根据分数段设置不同颜色
            if (range.startsWith("90") || range.startsWith("80")) {
                bar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }"); // 绿色
            } else if (range.startsWith("70") || range.startsWith("60")) {
                bar->setStyleSheet("QProgressBar::chunk { background-color: #FF9800; }"); // 橙色
            } else {
                bar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }"); // 红色
            }

            rowLayout->addWidget(label);
            rowLayout->addWidget(bar);
            layout->addWidget(row);

            qDebug() << "  分数段:" << range << "人数:" << count << "百分比:" << percentage << "%";
        }

        qDebug() << "成绩分布数据更新完成";

    } catch (...) {
        qDebug() << "更新分布数据时出错";
        QLabel *errorLabel = new QLabel("数据加载失败");
        errorLabel->setAlignment(Qt::AlignCenter);
        errorLabel->setStyleSheet("color: red; font-size: 14px;");
        distributionWidget->layout()->addWidget(errorLabel);
    }
}

void StatisticsDialog::updateTrendData()
{
    qDebug() << "正在更新趋势数据...";

    if (!trendWidget || !database) {
        qDebug() << "错误：控件或数据库为空";
        return;
    }

    try {
        QVector<QMap<QString, QVariant>> trendData = database->getTrendData();
        qDebug() << "从数据库获取到" << trendData.size() << "条趋势记录";

        // 清除旧内容
        QLayout *layout = trendWidget->layout();
        if (layout) {
            while (QLayoutItem* item = layout->takeAt(0)) {
                if (item->widget()) {
                    item->widget()->deleteLater();
                }
                delete item;
            }
        }

        if (trendData.isEmpty()) {
            QLabel *noDataLabel = new QLabel("暂无趋势数据");
            noDataLabel->setAlignment(Qt::AlignCenter);
            noDataLabel->setStyleSheet("color: gray; font-size: 14px;");
            layout->addWidget(noDataLabel);
            qDebug() << "没有趋势数据";
            return;
        }

        QLabel *title = new QLabel("各班级平均分趋势");
        title->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 10px;");
        title->setAlignment(Qt::AlignCenter);
        layout->addWidget(title);

        // 按总分排序
        QList<QPair<double, QMap<QString, QVariant>>> sortedData;
        for (const auto &data : trendData) {
            double chinese = data["chinese"].toDouble();
            double math = data["math"].toDouble();
            double english = data["english"].toDouble();
            double total = chinese + math + english;
            sortedData.append(qMakePair(total, data));
        }

        // 降序排序
        std::sort(sortedData.begin(), sortedData.end(),
                  [](const QPair<double, QMap<QString, QVariant>> &a,
                     const QPair<double, QMap<QString, QVariant>> &b) {
                      return a.first > b.first;
                  });

        for (int i = 0; i < sortedData.size(); i++) {
            const auto &data = sortedData[i].second;
            QString className = simplifyClassName(data["class"].toString());
            double chinese = data["chinese"].toDouble();
            double math = data["math"].toDouble();
            double english = data["english"].toDouble();
            double total = chinese + math + english;

            QString trendText = QString("%1. %2 - 语文: %3 | 数学: %4 | 英语: %5 | 总分: %6")
                                    .arg(i + 1)
                                    .arg(className)
                                    .arg(chinese, 0, 'f', 1)
                                    .arg(math, 0, 'f', 1)
                                    .arg(english, 0, 'f', 1)
                                    .arg(total, 0, 'f', 1);

            QLabel *label = new QLabel(trendText);
            label->setMargin(5);

            // 为前三名设置不同颜色
            if (i == 0) {
                label->setStyleSheet("color: #FFD700; font-weight: bold;"); // 金色
            } else if (i == 1) {
                label->setStyleSheet("color: #C0C0C0; font-weight: bold;"); // 银色
            } else if (i == 2) {
                label->setStyleSheet("color: #CD7F32; font-weight: bold;"); // 铜色
            }

            layout->addWidget(label);
            qDebug() << "  趋势:" << trendText;
        }

        qDebug() << "趋势数据更新完成";

    } catch (...) {
        qDebug() << "更新趋势数据时出错";
        QLabel *errorLabel = new QLabel("数据加载失败");
        errorLabel->setAlignment(Qt::AlignCenter);
        errorLabel->setStyleSheet("color: red; font-size: 14px;");
        trendWidget->layout()->addWidget(errorLabel);
    }
}

void StatisticsDialog::updateClassList()
{
    qDebug() << "正在更新班级列表...";

    if (!database) {
        qDebug() << "错误：数据库为空";
        return;
    }

    try {
        ui->classList->clear();
        ui->classList->addItem("全部");

        QStringList classes = database->getAllClasses();
        qDebug() << "从数据库获取到" << classes.size() << "个班级";

        for (const QString &className : classes) {
            QString simpleName = simplifyClassName(className);
            ui->classList->addItem(simpleName);
            qDebug() << "  添加班级:" << simpleName << "(原名称:" << className << ")";
        }

        ui->classList->setCurrentRow(0);
        qDebug() << "班级列表更新完成";

    } catch (...) {
        qDebug() << "更新班级列表时出错";
        ui->classList->clear();
        ui->classList->addItem("全部");
    }
}

void StatisticsDialog::on_subjectCombo_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    qDebug() << "科目已更改，重新加载数据...";
    updateAllData();
}

void StatisticsDialog::on_classList_currentTextChanged(const QString &currentText)
{
    qDebug() << "班级选择已更改为:" << currentText;
    updateStatsPanel(currentText);
}

void StatisticsDialog::updateStatsPanel(const QString &className)
{
    qDebug() << "正在更新统计指标...";

    if (!database) {
        qDebug() << "错误：数据库为空";
        return;
    }

    try {
        QString subject = ui->subjectCombo->currentText();
        QString selectedClass = (className == "全部" || className.isEmpty()) ? "" : className;

        // 先获取该科目的所有统计数据
        QVector<QMap<QString, QVariant>> allStats = database->getSubjectStats(subject);

        if (allStats.isEmpty()) {
            ui->avgValue->setText("0.0");
            ui->maxValue->setText("0");
            ui->minValue->setText("0");
            ui->passValue->setText("0%");
            ui->countValue->setText("0");
            qDebug() << "没有统计数据";
            return;
        }

        if (selectedClass.isEmpty()) {
            // 计算全部班级的统计指标
            double totalAvg = 0;
            double totalMax = 0;
            double totalMin = 100;
            int totalCount = 0;
            int totalPassCount = 0;

            for (const auto &stat : allStats) {
                double avgScore = stat["avg_score"].toDouble();
                double maxScore = stat["max_score"].toDouble();
                double minScore = stat["min_score"].toDouble();
                int count = stat["count"].toInt();
                double passRate = stat["pass_rate"].toDouble();

                totalAvg += avgScore * count;
                totalMax = qMax(totalMax, maxScore);
                totalMin = qMin(totalMin, minScore);
                totalCount += count;
                totalPassCount += qRound(passRate * count / 100.0);
            }

            double finalAvg = totalCount > 0 ? totalAvg / totalCount : 0;
            double finalPassRate = totalCount > 0 ? (totalPassCount * 100.0 / totalCount) : 0;

            ui->avgValue->setText(QString::number(finalAvg, 'f', 1));
            ui->maxValue->setText(QString::number(totalMax, 'f', 1));
            ui->minValue->setText(QString::number(totalMin, 'f', 1));
            ui->passValue->setText(QString::number(finalPassRate, 'f', 1) + "%");
            ui->countValue->setText(QString::number(totalCount));

            qDebug() << "全部班级统计 - 平均分:" << finalAvg << "最高分:" << totalMax
                     << "最低分:" << totalMin << "及格率:" << finalPassRate << "% 人数:" << totalCount;

        } else {
            // 查找指定班级的统计数据
            bool found = false;
            for (const auto &stat : allStats) {
                QString originalClassName = stat["class"].toString();
                QString simpleName = simplifyClassName(originalClassName);

                if (simpleName == selectedClass) {
                    ui->avgValue->setText(QString::number(stat["avg_score"].toDouble(), 'f', 1));
                    ui->maxValue->setText(QString::number(stat["max_score"].toDouble(), 'f', 1));
                    ui->minValue->setText(QString::number(stat["min_score"].toDouble(), 'f', 1));
                    ui->passValue->setText(QString::number(stat["pass_rate"].toDouble(), 'f', 1) + "%");
                    ui->countValue->setText(stat["count"].toString());
                    found = true;

                    qDebug() << "班级" << selectedClass << "统计 - 平均分:" << stat["avg_score"].toDouble()
                             << "最高分:" << stat["max_score"].toDouble() << "最低分:" << stat["min_score"].toDouble()
                             << "及格率:" << stat["pass_rate"].toDouble() << "% 人数:" << stat["count"].toInt();
                    break;
                }
            }

            if (!found) {
                ui->avgValue->setText("0.0");
                ui->maxValue->setText("0");
                ui->minValue->setText("0");
                ui->passValue->setText("0%");
                ui->countValue->setText("0");
                qDebug() << "未找到班级:" << selectedClass;
            }
        }

    } catch (...) {
        qDebug() << "更新统计指标时出错";
        ui->avgValue->setText("0.0");
        ui->maxValue->setText("0");
        ui->minValue->setText("0");
        ui->passValue->setText("0%");
        ui->countValue->setText("0");
    }
}

// 新增函数：简化班级名称（去掉"年级"）
QString StatisticsDialog::simplifyClassName(const QString &className)
{
    QString result = className;
    // 去掉"年级"字样
    result = result.replace("一年级", "一").replace("二年级", "二")
                 .replace("三年级", "三").replace("四年级", "四")
                 .replace("五年级", "五").replace("六年级", "六")
                 .replace("年级", "");

    // 如果只有数字，加上"班"
    if (result.toInt() > 0) {
        result = result + "班";
    }

    return result;
}
