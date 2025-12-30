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
                             "class TEXT NOT NULL,"
                             "chinese REAL DEFAULT -1,"       // 添加语文成绩字段
                             "math REAL DEFAULT -1,"         // 添加数学成绩字段
                             "english REAL DEFAULT -1,"      // 添加英语成绩字段
                             "total REAL GENERATED ALWAYS AS ("  // 添加总分计算字段
                             "  CASE WHEN chinese >= 0 THEN chinese ELSE 0 END +"
                             "  CASE WHEN math >= 0 THEN math ELSE 0 END +"
                             "  CASE WHEN english >= 0 THEN english ELSE 0 END"
                             ") VIRTUAL);";

    if (!query.exec(createTableSQL)) {
        qDebug() << "创建表失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "学生表创建成功";
    return true;
}



QVector<QMap<QString, QVariant>> Database::getAllStudents()
{
    QVector<QMap<QString, QVariant>> students;

    // 检查数据库是否打开
    if (!db.isOpen()) {
        qDebug() << "数据库未打开";
        return students;
    }

    QSqlQuery query("SELECT * FROM students ORDER BY class, stu_id");

    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        return students;
    }

    while (query.next()) {
        QMap<QString, QVariant> student;
        student["id"] = query.value("id");
        student["stu_id"] = query.value("stu_id");
        student["name"] = query.value("name");
        student["class"] = query.value("class");
        students.append(student);
    }

    qDebug() << "查询到" << students.size() << "名学生";
    return students;
}

QVector<QMap<QString, QVariant>> Database::searchStudents(const QString &keyword)
{
    QVector<QMap<QString, QVariant>> students;

    if (!db.isOpen()) {
        qDebug() << "数据库未打开";
        return students;
    }

    QSqlQuery query;
    QString sql = "SELECT * FROM students WHERE stu_id LIKE ? OR name LIKE ? OR class LIKE ? "
                  "ORDER BY class, stu_id";

    if (!query.prepare(sql)) {
        qDebug() << "准备查询失败:" << query.lastError().text();
        return students;
    }

    QString pattern = "%" + keyword + "%";
    query.addBindValue(pattern);
    query.addBindValue(pattern);
    query.addBindValue(pattern);

    if (!query.exec()) {
        qDebug() << "执行查询失败:" << query.lastError().text();
        return students;
    }

    while (query.next()) {
        QMap<QString, QVariant> student;
        student["id"] = query.value("id");
        student["stu_id"] = query.value("stu_id");
        student["name"] = query.value("name");
        student["class"] = query.value("class");
        student["chinese"] = query.value("chinese");
        student["math"] = query.value("math");
        student["english"] = query.value("english");
        student["total"] = query.value("total");
        students.append(student);
    }

    qDebug() << "搜索到" << students.size() << "名学生";
    return students;
}

bool Database::deleteStudent(const QString &stuId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE stu_id = ?");
    query.addBindValue(stuId);

    if (!query.exec()) {
        qDebug() << "删除失败:" << query.lastError().text();
        return false;
    }

    qDebug() << "删除学生:" << stuId;
    return query.numRowsAffected() > 0;
}

bool Database::isStudentExist(const QString &stuId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM students WHERE stu_id = ?");
    query.addBindValue(stuId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}
