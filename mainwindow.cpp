#include "mainwindow.h"
#include <QLabel>
#include "ui_mainwindow.h"  // 添加这行

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)  // 初始化ui指针
{
    ui->setupUi(this);  // 设置UI
}

MainWindow::~MainWindow()
{
}
