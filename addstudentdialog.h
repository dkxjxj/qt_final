#ifndef ADDSTUDENTDIALOG_H
#define ADDSTUDENTDIALOG_H

#include <QDialog>
#include "database.h"

namespace Ui {
class AddStudentDialog;
}

class AddStudentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddStudentDialog(QWidget *parent = nullptr, Database *db = nullptr);
    ~AddStudentDialog();

private slots:
    void on_buttonBox_accepted();    // 修改：从 on_addButton_clicked 改为 on_buttonBox_accepted
    void on_buttonBox_rejected();    // 修改：从 on_cancelButton_clicked 改为 on_buttonBox_rejected

private:
    bool validateInput();

    Ui::AddStudentDialog *ui;
    Database *database;
};

#endif // ADDSTUDENTDIALOG_H
