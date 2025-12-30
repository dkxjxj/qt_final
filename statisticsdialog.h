#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include "database.h"

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
    void on_classList_currentTextChanged(const QString &currentText);

private:
    void setupWidgets();
    void updateAllData();
    void updateClassData();
    void updateTrendData();
    void updateClassList();

    Ui::StatisticsDialog *ui;
    Database *database;

    QTableWidget *classTable;  // 保持与UI一致
    QWidget *trendWidget;      // 保持与UI一致
};

#endif // STATISTICSDIALOG_H
