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
    QSqlQuery query;

    QString createTableSQL = "CREATE TABLE IF NOT EXISTS students ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "stu_id TEXT UNIQUE NOT NULL,"
                             "name TEXT NOT NULL,"
                             "class TEXT NOT NULL);";

    if (!query.exec(createTableSQL)) {
        qDebug() << "创建表失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "学生表创建成功";
    return true;
}
