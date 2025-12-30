#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QColor>
#include <QTableWidgetItem>
#include <algorithm>

StatisticsDialog::StatisticsDialog(QWidget *parent, Database *db)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , database(db)
    , classTable(nullptr)
    , trendWidget(nullptr)
{
    // 先检查数据库
    if (!database) {
        QMessageBox::critical(parent, "错误", "数据库连接无效！");
        return;
    }

    try {
        ui->setupUi(this);
        setWindowTitle("成绩统计分析");
        setMinimumSize(800, 600);

        // 只初始化有数据的控件
        setupWidgets();

        // 连接信号槽
        connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &StatisticsDialog::reject);

        // 初始化数据
        updateClassList();
        updateAllData();

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "初始化错误",
                              QString("对话框初始化失败：%1\n请检查数据库连接和数据。").arg(e.what()));
    }
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::setupWidgets()
{
    // 1. 班级对比表格（有数据）
    classTable = new QTableWidget();
    classTable->setColumnCount(5);
    QStringList classHeaders = {"班级", "语文", "数学", "英语", "总分"};
    classTable->setHorizontalHeaderLabels(classHeaders);
    classTable->verticalHeader()->setVisible(false);
    classTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    classTable->setStyleSheet("QTableWidget { font-size: 12pt; }");

    QVBoxLayout *layout1 = new QVBoxLayout(ui->classGroup);
    layout1->addWidget(classTable);

    // 2. 趋势分析控件（有数据）
    trendWidget = new QWidget();
    QVBoxLayout *layout2 = new QVBoxLayout(trendWidget);
    layout2->setAlignment(Qt::AlignTop);
    layout2->setSpacing(10);

    QVBoxLayout *groupLayout2 = new QVBoxLayout(ui->trendGroup);
    groupLayout2->addWidget(trendWidget);
}

void StatisticsDialog::updateAllData()
{
    updateClassData();
    updateTrendData();
}

void StatisticsDialog::updateClassData()
{
    if (!classTable || !database) return;

    classTable->clearContents();

    try {
        QVector<QMap<QString, QVariant>> stats = database->getClassStats();

        if (stats.isEmpty()) {
            classTable->setRowCount(1);
            QTableWidgetItem *noDataItem = new QTableWidgetItem("暂无班级数据");
            noDataItem->setTextAlignment(Qt::AlignCenter);
            classTable->setItem(0, 0, noDataItem);
            classTable->setSpan(0, 0, 1, 5);
            return;
        }

        classTable->setRowCount(stats.size());

        for (int i = 0; i < stats.size(); i++) {
            const auto &stat = stats[i];

            QTableWidgetItem *classItem = new QTableWidgetItem(stat["class"].toString());
            QTableWidgetItem *chineseItem = new QTableWidgetItem(QString::number(stat["chinese_avg"].toDouble(), 'f', 1));
            QTableWidgetItem *mathItem = new QTableWidgetItem(QString::number(stat["math_avg"].toDouble(), 'f', 1));
            QTableWidgetItem *englishItem = new QTableWidgetItem(QString::number(stat["english_avg"].toDouble(), 'f', 1));

            // 计算总分
            double total = stat["chinese_avg"].toDouble() +
                           stat["math_avg"].toDouble() +
                           stat["english_avg"].toDouble();
            QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(total, 'f', 1));

            // 设置颜色
            if (total >= 240) {
                classItem->setBackground(QColor(220, 255, 220));
                chineseItem->setBackground(QColor(220, 255, 220));
                mathItem->setBackground(QColor(220, 255, 220));
                englishItem->setBackground(QColor(220, 255, 220));
                totalItem->setBackground(QColor(220, 255, 220));
            } else if (total < 180) {
                classItem->setBackground(QColor(255, 220, 220));
                chineseItem->setBackground(QColor(255, 220, 220));
                mathItem->setBackground(QColor(255, 220, 220));
                englishItem->setBackground(QColor(255, 220, 220));
                totalItem->setBackground(QColor(255, 220, 220));
            }

            classTable->setItem(i, 0, classItem);
            classTable->setItem(i, 1, chineseItem);
            classTable->setItem(i, 2, mathItem);
            classTable->setItem(i, 3, englishItem);
            classTable->setItem(i, 4, totalItem);
        }

        classTable->resizeColumnsToContents();

    } catch (...) {
        qDebug() << "updateClassData 出错";
    }
}

void StatisticsDialog::updateTrendData()
{
    if (!trendWidget || !database) return;

    // 清除旧内容
    QLayoutItem *item;
    while ((item = trendWidget->layout()->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    try {
        QVector<QMap<QString, QVariant>> trendData = database->getTrendData();

        if (trendData.isEmpty()) {
            QLabel *noDataLabel = new QLabel("暂无趋势数据");
            noDataLabel->setAlignment(Qt::AlignCenter);
            noDataLabel->setStyleSheet("font-size: 14pt; color: gray;");
            trendWidget->layout()->addWidget(noDataLabel);
            return;
        }

        // 创建标题
        QLabel *titleLabel = new QLabel("各班级平均分排名");
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14pt; margin-bottom: 15px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        trendWidget->layout()->addWidget(titleLabel);

        // 按总分排序
        std::sort(trendData.begin(), trendData.end(),
                  [](const QMap<QString, QVariant> &a, const QMap<QString, QVariant> &b) {
                      double totalA = a["chinese"].toDouble() + a["math"].toDouble() + a["english"].toDouble();
                      double totalB = b["chinese"].toDouble() + b["math"].toDouble() + b["english"].toDouble();
                      return totalA > totalB;
                  });

        // 显示每个班级的数据
        for (int i = 0; i < trendData.size(); i++) {
            const auto &data = trendData[i];
            QString className = data["class"].toString();
            double chinese = data["chinese"].toDouble();
            double math = data["math"].toDouble();
            double english = data["english"].toDouble();
            double total = chinese + math + english;

            QWidget *rowWidget = new QWidget();
            QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
            rowLayout->setContentsMargins(10, 5, 10, 5);

            // 排名标签
            QLabel *rankLabel = new QLabel(QString("%1.").arg(i + 1));
            rankLabel->setFixedWidth(40);
            rankLabel->setAlignment(Qt::AlignCenter);

            // 班级名称
            QLabel *classLabel = new QLabel(className);
            classLabel->setFixedWidth(100);

            // 成绩信息
            QString scores = QString("语文:%1 数学:%2 英语:%3")
                                 .arg(chinese, 0, 'f', 1)
                                 .arg(math, 0, 'f', 1)
                                 .arg(english, 0, 'f', 1);
            QLabel *scoreLabel = new QLabel(scores);
            scoreLabel->setMinimumWidth(200);

            // 总分
            QLabel *totalLabel = new QLabel(QString("总分:%1").arg(total, 0, 'f', 1));
            totalLabel->setFixedWidth(100);
            totalLabel->setAlignment(Qt::AlignRight);

            // 设置样式
            QString style = "font-size: 11pt; padding: 5px;";
            if (i == 0) {
                style += "font-weight: bold; color: #d48806; background: #fffbe6; border: 1px solid #ffd591; border-radius: 4px;";
            } else if (i == 1) {
                style += "color: #595959; background: #fafafa; border: 1px solid #d9d9d9; border-radius: 4px;";
            } else if (i == 2) {
                style += "color: #8c8c8c; background: #f5f5f5; border: 1px solid #d9d9d9; border-radius: 4px;";
            } else {
                style += "color: #8c8c8c;";
            }

            rankLabel->setStyleSheet(style);
            classLabel->setStyleSheet(style);
            scoreLabel->setStyleSheet(style);
            totalLabel->setStyleSheet(style);

            rowLayout->addWidget(rankLabel);
            rowLayout->addWidget(classLabel);
            rowLayout->addWidget(scoreLabel);
            rowLayout->addWidget(totalLabel);
            rowLayout->addStretch();

            trendWidget->layout()->addWidget(rowWidget);
        }

        // 添加说明
        QLabel *noteLabel = new QLabel("说明：按班级总分平均分排名，绿色表示优秀(≥240)，红色表示需关注(<180)");
        noteLabel->setStyleSheet("font-size: 10pt; color: #666; margin-top: 20px; padding: 5px;");
        noteLabel->setAlignment(Qt::AlignCenter);
        trendWidget->layout()->addWidget(noteLabel);

    } catch (...) {
        qDebug() << "updateTrendData 出错";
    }
}

void StatisticsDialog::updateClassList()
{
    try {
        // 检查UI中是否有classList控件
        // 如果没有这个控件，就跳过这个函数
        // qDebug() << "updateClassList被调用，但UI中可能没有classList控件";

    } catch (...) {
        qDebug() << "updateClassList 出错";
    }
}

void StatisticsDialog::on_classList_currentTextChanged(const QString &currentText)
{
    Q_UNUSED(currentText);
    // 暂时不处理班级选择变化
}
