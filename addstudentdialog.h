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
    void on_addButton_clicked();
    void on_cancelButton_clicked();

private:
    bool validateInput();

    Ui::AddStudentDialog *ui;
    Database *database;
};

#endif // ADDSTUDENTDIALOG_H
