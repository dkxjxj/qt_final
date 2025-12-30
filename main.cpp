#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("学生成绩分析系统");
    app.setOrganizationName("School");

    MainWindow window;
    window.show();

    return app.exec();
}
