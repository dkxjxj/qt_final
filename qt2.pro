QT += core gui sql
# 去掉 charts，因为我们不使用图表模块

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++17  # Qt 6需要C++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    studentmodel.cpp \
    addstudentdialog.cpp \
    statisticsdialog.cpp

HEADERS += \
    mainwindow.h \
    database.h \
    studentmodel.h \
    addstudentdialog.h \
    statisticsdialog.h

FORMS += \
    mainwindow.ui \
    addstudentdialog.ui \
    statisticsdialog.ui

# Release模式配置
CONFIG(release, debug|release) {
    DESTDIR = $$PWD/release
}

# Debug模式配置
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/debug
}

TARGET = StudentGradeSystem
