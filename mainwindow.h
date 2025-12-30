#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"
#include "studentmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 菜单栏动作
    void on_actionAdd_triggered();
    void on_actionDelete_triggered();
    void on_actionRefresh_triggered();
    void on_actionExit_triggered();

    // 工具栏按钮
    void on_searchButton_clicked();
    void on_clearButton_clicked();

private:
    Ui::MainWindow *ui;
    Database db;
    StudentModel *studentModel;

    void loadStudentData();
};

#endif // MAINWINDOW_H
