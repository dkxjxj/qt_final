#ifndef ADDSTUDENTDIALOG_H
#define ADDSTUDENTDIALOG_H

#include <QDialog>

namespace Ui {
class AddStudentDialog;
}

class AddStudentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddStudentDialog(QWidget *parent = nullptr);
    ~AddStudentDialog();

private slots:
    void on_addButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AddStudentDialog *ui;
};

#endif // ADDSTUDENTDIALOG_H
