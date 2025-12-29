#include "database.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

Database::Database(QObject *parent) : QObject(parent)
{
}

Database::~Database()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool Database::openDatabase()
{

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    if (!db.open()) {
        qDebug() << "无法打开数据库：" << db.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功！";
    return true;
}

bool Database::createTables()
{

    return true;
}
