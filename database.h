#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QVector>
#include <QMap>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    // 只声明，不实现
    bool openDatabase();
    bool createTables();

    bool addStudent(const QString &stuId, const QString &name, const QString &className,
                    double chinese, double math, double english);

    // 添加更新学生方法
    bool updateStudent(const QString &stuId, const QString &name, const QString &className,
                       double chinese, double math, double english);

    bool deleteStudent(const QString &stuId);
    bool isStudentExist(const QString &stuId);
    bool deleteStudent(const QString &stuId);
    bool isStudentExist(const QString &stuId);


    QVector<QMap<QString, QVariant>> getAllStudents();
    QVector<QMap<QString, QVariant>> searchStudents(const QString &keyword);
    // 在public部分添加
    QVector<QMap<QString, QVariant>> searchStudents(const QString &keyword);
private:
    QSqlDatabase db;
};

#endif // DATABASE_H

