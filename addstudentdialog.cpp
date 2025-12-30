#include "addstudentdialog.h"
#include "ui_addstudentdialog.h"
#include <QMessageBox>
#include <QDebug>

AddStudentDialog::AddStudentDialog(QWidget *parent, Database *db)
    : QDialog(parent)
    , ui(new Ui::AddStudentDialog)
    , database(db)
{
    ui->setupUi(this);
    setWindowTitle("添加学生");

    // 设置默认值
    ui->chineseEdit->setText("0");
    ui->mathEdit->setText("0");
    ui->englishEdit->setText("0");

    // 注意：由于使用了 buttonBox 的 accepted()/rejected() 信号
    // 不需要手动连接，Qt会自动连接标准按钮的信号
}

AddStudentDialog::~AddStudentDialog()
{
    delete ui;
}

bool AddStudentDialog::validateInput()
{
    // 检查学号
    QString stuId = ui->stuIdEdit->text().trimmed();
    if (stuId.isEmpty()) {
        QMessageBox::warning(this, "警告", "学号不能为空！");
        return false;
    }

    if (database->isStudentExist(stuId)) {
        QMessageBox::warning(this, "警告", "学号已存在！");
        return false;
    }

    // 检查姓名
    QString name = ui->nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "警告", "姓名不能为空！");
        return false;
    }

    // 检查班级
    QString className = ui->classEdit->text().trimmed();
    if (className.isEmpty()) {
        QMessageBox::warning(this, "警告", "班级不能为空！");
        return false;
    }

    // 检查成绩
    bool ok;
    double chinese = ui->chineseEdit->text().toDouble(&ok);
    if (!ok || chinese < 0 || chinese > 100) {
        QMessageBox::warning(this, "警告", "语文成绩必须是0-100的数字！");
        return false;
    }

    double math = ui->mathEdit->text().toDouble(&ok);
    if (!ok || math < 0 || math > 100) {
        QMessageBox::warning(this, "警告", "数学成绩必须是0-100的数字！");
        return false;
    }

    double english = ui->englishEdit->text().toDouble(&ok);
    if (!ok || english < 0 || english > 100) {
        QMessageBox::warning(this, "警告", "英语成绩必须是0-100的数字！");
        return false;
    }

    return true;
}

void AddStudentDialog::on_buttonBox_accepted()  // 修改：从 on_addButton_clicked 改为 on_buttonBox_accepted
{
    if (!validateInput()) {
        // 阻止对话框关闭
        return;
    }

    QString stuId = ui->stuIdEdit->text().trimmed();
    QString name = ui->nameEdit->text().trimmed();
    QString className = ui->classEdit->text().trimmed();
    double chinese = ui->chineseEdit->text().toDouble();
    double math = ui->mathEdit->text().toDouble();
    double english = ui->englishEdit->text().toDouble();

    if (database->addStudent(stuId, name, className, chinese, math, english)) {
        QMessageBox::information(this, "成功", "学生添加成功！");
        // accept() 会自动调用，因为这是 buttonBox 的 accepted 信号
    } else {
        QMessageBox::critical(this, "错误", "添加失败！");
        // 阻止对话框关闭
        return;
    }
}

void AddStudentDialog::on_buttonBox_rejected()  // 修改：从 on_cancelButton_clicked 改为 on_buttonBox_rejected
{
    // reject() 会自动调用，因为这是 buttonBox 的 rejected 信号
}
