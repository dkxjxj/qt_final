#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addstudentdialog.h"
#include "statisticsdialog.h"
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化数据库
    if (!db.openDatabase()) {
        QMessageBox::critical(this, "错误", "无法打开数据库！");
        return;
    }

    // 初始化模型
    studentModel = new StudentModel(this);
    ui->tableView->setModel(studentModel);

    // 设置表格属性
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();

    // 连接信号槽
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateStatusBar);

    // 加载数据
    loadStudentData();
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置窗口标题
    setWindowTitle("学生成绩分析系统");

    // 设置状态栏
    updateStatusBar();
}

void MainWindow::loadStudentData()
{
    QVector<QMap<QString, QVariant>> students = db.getAllStudents();
    studentModel->setData(students);
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::updateStatusBar()
{
    int total = studentModel->rowCount();
    int selected = ui->tableView->selectionModel()->selectedRows().size();
    ui->statusbar->showMessage(QString("共 %1 名学生 | 选中 %2 名").arg(total).arg(selected));
}

void MainWindow::on_actionAdd_triggered()
{
    AddStudentDialog dialog(this, &db);
    if (dialog.exec() == QDialog::Accepted) {
        loadStudentData();
    }
}

void MainWindow::on_actionDelete_triggered()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的学生！");
        return;
    }

    int row = selected.first().row();
    QMap<QString, QVariant> student = studentModel->getStudent(row);
    QString stuId = student["stu_id"].toString();
    QString name = student["name"].toString();

    int ret = QMessageBox::question(this, "确认删除",
                                    QString("确定要删除学生 %1 (%2) 吗？").arg(name).arg(stuId),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (db.deleteStudent(stuId)) {
            loadStudentData();
            QMessageBox::information(this, "成功", "学生删除成功！");
        } else {
            QMessageBox::critical(this, "错误", "删除失败！");
        }
    }
}

void MainWindow::on_actionRefresh_triggered()
{
    loadStudentData();
}

void MainWindow::on_actionStatistics_triggered()
{
    StatisticsDialog dialog(this, &db);
    dialog.exec();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_searchButton_clicked()
{
    QString keyword = ui->searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        loadStudentData();
        return;
    }

    QVector<QMap<QString, QVariant>> students = db.searchStudents(keyword);
    studentModel->setData(students);
    updateStatusBar();
}

void MainWindow::on_clearButton_clicked()
{
    ui->searchEdit->clear();
    loadStudentData();
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    int row = index.row();
    QMap<QString, QVariant> student = studentModel->getStudent(row);

    QString info = QString("学生信息：\n"
                           "学号：%1\n"
                           "姓名：%2\n"
                           "班级：%3\n"
                           "语文：%4\n"
                           "数学：%5\n"
                           "英语：%6\n"
                           "总分：%7\n"
                           "平均分：%8")
                       .arg(student["stu_id"].toString())
                       .arg(student["name"].toString())
                       .arg(student["class"].toString())
                       .arg(student["chinese"].isNull() ? "未录入" : student["chinese"].toString())
                       .arg(student["math"].isNull() ? "未录入" : student["math"].toString())
                       .arg(student["english"].isNull() ? "未录入" : student["english"].toString())
                       .arg(student["total"].toString())
                       .arg(student["average"].toString());

    QMessageBox::information(this, "学生详情", info);
}
