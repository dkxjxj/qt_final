#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"      // 确保包含
#include "studentmodel.h"  // 确保包含
#include <QMessageBox>
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    // 在这里添加 ▼ - 初始化studentModel
    , studentModel(new StudentModel(this))
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("学生成绩分析系统");

    // 在这里添加 ▼ - 初始化数据库
    if (!db.openDatabase()) {
        QMessageBox::critical(this, "错误", "无法打开数据库！");
        return;
    }

    // 在这里添加 ▼ - 创建表
    if (!db.createTables()) {
        QMessageBox::critical(this, "错误", "创建数据表失败！");
        return;
    }

    // 在这里添加 ▼ - 设置表格模型
    ui->tableView->setModel(studentModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // 在这里添加 ▼ - 添加一些测试数据（可选）
    db.addStudent("1001", "张三", "一年级一班");
    db.addStudent("1002", "李四", "一年级一班");
    db.addStudent("1003", "王五", "一年级二班");

    // 在这里添加 ▼ - 加载学生数据
    loadStudentData();

    qDebug() << "主窗口初始化完成";
}


MainWindow::~MainWindow()
{
    delete ui;
    // 注意：studentModel是this的子对象，会自动销毁
}

// 在这里添加 ▼ - loadStudentData方法的实现
void MainWindow::loadStudentData()
{
    // 从数据库获取所有学生
    QVector<QMap<QString, QVariant>> students = db.getAllStudents();

    // 设置到模型中
    studentModel->setData(students);

    // 调整列宽
    ui->tableView->resizeColumnsToContents();

    // 显示状态信息
    qDebug() << "已加载" << students.size() << "名学生";
}

