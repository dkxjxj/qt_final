#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 直接创建并显示一个简单窗口
    QMainWindow window;
    window.setWindowTitle("学生成绩分析系统");
    window.resize(400, 300);
    window.show();

    return app.exec();
}
