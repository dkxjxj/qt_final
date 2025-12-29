QT += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++17  # Qt 6需要C++17

SOURCES += \
    database.cpp \
    main.cpp \
    mainwindow.cpp \
    studentmodel.cpp


HEADERS += \
    database.h \
    mainwindow.h \
    studentmodel.h


FORMS += \
    mainwindow.ui \

# Release模式配置
CONFIG(release, debug|release) {
    DESTDIR = $$PWD/release
}

# Debug模式配置
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/debug
}

TARGET = StudentGradeSystem
