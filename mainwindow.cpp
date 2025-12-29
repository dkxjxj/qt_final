#include "mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle("学生成绩分析系统");
    resize(800, 600);


    QLabel *label = new QLabel("学生成绩分析系统", this);
    label->setAlignment(Qt::AlignCenter);
    label->setGeometry(100, 100, 600, 50);
}

MainWindow::~MainWindow()
{
}
