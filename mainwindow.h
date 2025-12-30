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

private slots:  // 添加这行
    void on_actionAdd_triggered();  // 添加这行

private:
    Ui::MainWindow *ui;
    Database db;
    StudentModel *studentModel;

    void loadStudentData();
};

#endif // MAINWINDOW_H
