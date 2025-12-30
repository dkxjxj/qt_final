#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addstudentdialog.h"  // 添加这行
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , studentModel(new StudentModel(this))
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("学生成绩分析系统");

    // 初始化数据库
    if (!db.openDatabase()) {
        QMessageBox::critical(this, "错误", "无法打开数据库！");
        return;
    }

    // 创建表
    if (!db.createTables()) {
        QMessageBox::critical(this, "错误", "创建数据表失败！");
        return;
    }

    // 设置表格模型
    ui->tableView->setModel(studentModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // 修改这里：使用6个参数的addStudent函数
    // 参数顺序：学号, 姓名, 班级, 语文成绩, 数学成绩, 英语成绩
    db.addStudent("1001", "张三", "一年级一班", 85.5, 90.0, 78.5);
    db.addStudent("1002", "李四", "一年级一班", 92.0, 88.5, 95.0);
    db.addStudent("1003", "王五", "一年级二班", 76.0, 82.5, 70.0);

    // 加载学生数据
    loadStudentData();

    qDebug() << "主窗口初始化完成";
}

MainWindow::~MainWindow()
{
    delete ui;
    // 注意：studentModel是this的子对象，会自动销毁
}

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

// 添加这个槽函数实现
void MainWindow::on_actionAdd_triggered()
{
    AddStudentDialog dialog(this, &db);
    if (dialog.exec() == QDialog::Accepted) {
        loadStudentData();  // 刷新数据
    }
}
