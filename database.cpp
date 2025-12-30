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
    // ================ 重要：修改这里的路径 ================
    // 替换成你在Navicat中创建的数据库文件完整路径
    // 注意Windows路径要用双斜杠或正斜杠

    // 示例1：绝对路径（推荐）
    QString dbPath = "D:/StudentData/student_grade.db";
    // 或 QString dbPath = "D:\\StudentData\\student_grade.db";

    // 示例2：如果放在项目文件夹内
    // QString dbPath = QCoreApplication::applicationDirPath() + "/student_grade.db";

    // 打印路径，方便调试
    qDebug() << "数据库路径：" << dbPath;

    // ================ 验证文件是否存在 ================
    QFile file(dbPath);
    if (!file.exists()) {
        qDebug() << "错误：数据库文件不存在！路径：" << dbPath;
        qDebug() << "请先在Navicat中创建数据库文件";
        return false;
    }

    // ================ 连接数据库 ================
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "无法打开数据库：" << db.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功！";

    // 检查表是否存在
    QSqlQuery query;
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='students'")) {
        qDebug() << "检查表失败：" << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        qDebug() << "表'students'不存在，正在创建...";
        return createTables();  // 如果表不存在，创建表
    }

    qDebug() << "表已存在，直接使用";
    return true;
}

bool Database::createTables()
{
    QSqlQuery query;

    // 创建学生表
    QString createTableSQL = "CREATE TABLE IF NOT EXISTS students ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "stu_id TEXT UNIQUE NOT NULL,"
                             "name TEXT NOT NULL,"
                             "class TEXT NOT NULL,"
                             "chinese REAL DEFAULT -1,"
                             "math REAL DEFAULT -1,"
                             "english REAL DEFAULT -1,"
                             "total REAL GENERATED ALWAYS AS ("
                             "  CASE WHEN chinese >= 0 THEN chinese ELSE 0 END +"
                             "  CASE WHEN math >= 0 THEN math ELSE 0 END +"
                             "  CASE WHEN english >= 0 THEN english ELSE 0 END"
                             ") VIRTUAL,"
                             "average REAL GENERATED ALWAYS AS ("
                             "  (CASE WHEN chinese >= 0 THEN 1 ELSE 0 END +"
                             "   CASE WHEN math >= 0 THEN 1 ELSE 0 END +"
                             "   CASE WHEN english >= 0 THEN 1 ELSE 0 END) * 1.0 /"
                             "  NULLIF((CASE WHEN chinese >= 0 THEN chinese ELSE 0 END +"
                             "         CASE WHEN math >= 0 THEN math ELSE 0 END +"
                             "         CASE WHEN english >= 0 THEN english ELSE 0 END), 0)"
                             ") VIRTUAL"
                             ");";

    if (!query.exec(createTableSQL)) {
        qDebug() << "创建表失败:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Database::addStudent(const QString &stuId, const QString &name, const QString &className,
                          double chinese, double math, double english)
{
    QSqlQuery query;
    query.prepare("INSERT INTO students (stu_id, name, class, chinese, math, english) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(stuId);
    query.addBindValue(name);
    query.addBindValue(className);
    query.addBindValue(chinese >= 0 ? chinese : QVariant());
    query.addBindValue(math >= 0 ? math : QVariant());
    query.addBindValue(english >= 0 ? english : QVariant());

    return query.exec();
}

bool Database::updateStudent(const QString &stuId, const QString &name, const QString &className,
                             double chinese, double math, double english)
{
    QSqlQuery query;
    query.prepare("UPDATE students SET name = ?, class = ?, chinese = ?, math = ?, english = ? "
                  "WHERE stu_id = ?");
    query.addBindValue(name);
    query.addBindValue(className);
    query.addBindValue(chinese >= 0 ? chinese : QVariant());
    query.addBindValue(math >= 0 ? math : QVariant());
    query.addBindValue(english >= 0 ? english : QVariant());
    query.addBindValue(stuId);

    return query.exec();
}

bool Database::deleteStudent(const QString &stuId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE stu_id = ?");
    query.addBindValue(stuId);

    return query.exec();
}

QVector<QMap<QString, QVariant>> Database::getAllStudents()
{
    QVector<QMap<QString, QVariant>> students;
    QSqlQuery query("SELECT * FROM students ORDER BY class, stu_id");

    while (query.next()) {
        QMap<QString, QVariant> student;

        // Qt 6的正确写法：通过字段名获取数据
        student["id"] = query.value("id");
        student["stu_id"] = query.value("stu_id");
        student["name"] = query.value("name");
        student["class"] = query.value("class");
        student["chinese"] = query.value("chinese");
        student["math"] = query.value("math");
        student["english"] = query.value("english");
        student["total"] = query.value("total");
        student["average"] = query.value("average");

        students.append(student);
    }

    return students;
}

QVector<QMap<QString, QVariant>> Database::searchStudents(const QString &keyword)
{
    QVector<QMap<QString, QVariant>> students;
    QSqlQuery query;
    query.prepare("SELECT * FROM students WHERE stu_id LIKE ? OR name LIKE ? OR class LIKE ? "
                  "ORDER BY class, stu_id");
    QString pattern = "%" + keyword + "%";
    query.addBindValue(pattern);
    query.addBindValue(pattern);
    query.addBindValue(pattern);

    if (query.exec()) {
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
            student["average"] = query.value("average");
            students.append(student);
        }
    }

    return students;
}

QVector<QMap<QString, QVariant>> Database::getSubjectStats(const QString &subject)
{
    QVector<QMap<QString, QVariant>> stats;
    QSqlQuery query;

    QString sql = QString("SELECT class, "
                          "COUNT(*) as count, "
                          "AVG(%1) as avg_score, "
                          "MAX(%1) as max_score, "
                          "MIN(%1) as min_score, "
                          "SUM(CASE WHEN %1 >= 60 THEN 1 ELSE 0 END) * 100.0 / COUNT(*) as pass_rate "
                          "FROM students WHERE %1 >= 0 GROUP BY class").arg(subject);

    if (query.exec(sql)) {
        while (query.next()) {
            QMap<QString, QVariant> stat;
            stat["class"] = query.value("class");
            stat["count"] = query.value("count");
            stat["avg_score"] = query.value("avg_score");
            stat["max_score"] = query.value("max_score");
            stat["min_score"] = query.value("min_score");
            stat["pass_rate"] = query.value("pass_rate");
            stats.append(stat);
        }
    }

    return stats;
}

QVector<QMap<QString, QVariant>> Database::getClassStats()
{
    QVector<QMap<QString, QVariant>> stats;
    QSqlQuery query;

    QString sql = "SELECT class, "
                  "COUNT(*) as total_students, "
                  "AVG(CASE WHEN chinese >= 0 THEN chinese ELSE NULL END) as chinese_avg, "
                  "AVG(CASE WHEN math >= 0 THEN math ELSE NULL END) as math_avg, "
                  "AVG(CASE WHEN english >= 0 THEN english ELSE NULL END) as english_avg, "
                  "AVG(total) as total_avg "
                  "FROM students GROUP BY class ORDER BY total_avg DESC";

    if (query.exec(sql)) {
        while (query.next()) {
            QMap<QString, QVariant> stat;
            stat["class"] = query.value("class");
            stat["total_students"] = query.value("total_students");
            stat["chinese_avg"] = query.value("chinese_avg");
            stat["math_avg"] = query.value("math_avg");
            stat["english_avg"] = query.value("english_avg");
            stat["total_avg"] = query.value("total_avg");
            stats.append(stat);
        }
    }

    return stats;
}

QVector<QMap<QString, QVariant>> Database::getScoreDistribution(const QString &subject)
{
    QVector<QMap<QString, QVariant>> distribution;

    // 定义分数段
    QStringList ranges = {"0-59", "60-69", "70-79", "80-89", "90-100"};

    for (const QString &range : ranges) {
        QStringList parts = range.split("-");
        int min = parts[0].toInt();
        int max = parts[1].toInt();

        QSqlQuery query;
        query.prepare(QString("SELECT COUNT(*) as count FROM students "
                              "WHERE %1 >= ? AND %1 <= ?").arg(subject));
        query.addBindValue(min);
        query.addBindValue(max);

        if (query.exec() && query.next()) {
            QMap<QString, QVariant> item;
            item["range"] = range;
            item["count"] = query.value("count");
            distribution.append(item);
        }
    }

    return distribution;
}

QVector<QMap<QString, QVariant>> Database::getTrendData()
{
    QVector<QMap<QString, QVariant>> trendData;
    QSqlQuery query;

    // 获取各班级平均分趋势
    QString sql = "SELECT class, "
                  "AVG(chinese) as chinese, "
                  "AVG(math) as math, "
                  "AVG(english) as english "
                  "FROM students "
                  "WHERE chinese >= 0 AND math >= 0 AND english >= 0 "
                  "GROUP BY class "
                  "ORDER BY class";

    if (query.exec(sql)) {
        while (query.next()) {
            QMap<QString, QVariant> data;
            data["class"] = query.value("class");
            data["chinese"] = query.value("chinese");
            data["math"] = query.value("math");
            data["english"] = query.value("english");
            trendData.append(data);
        }
    }

    return trendData;
}

QStringList Database::getAllClasses()
{
    QStringList classes;
    QSqlQuery query("SELECT DISTINCT class FROM students ORDER BY class");

    while (query.next()) {
        classes.append(query.value(0).toString());
    }

    return classes;
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
