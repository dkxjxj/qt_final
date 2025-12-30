#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "database.h"
#include <QTableWidget>
#include <QWidget>

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
    void on_subjectCombo_currentIndexChanged(int index);
    void on_classList_currentTextChanged(const QString &currentText);

private:
    void initializeWidgets();
    void updateAllData();
    void updateClassList();
    void updateStatsPanel(const QString &className = "");
    void updateSubjectData();
    void updateClassData();
    void updateDistributionData();
    void updateTrendData();

    // 新增函数：简化班级名称
    QString simplifyClassName(const QString &className);

    Ui::StatisticsDialog *ui;
    Database *database;
    QTableWidget *subjectTable;
    QTableWidget *classTable;
    QWidget *distributionWidget;
    QWidget *trendWidget;
};

#endif // STATISTICSDIALOG_H
