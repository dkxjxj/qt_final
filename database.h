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

    bool openDatabase();
    bool createTables();

    // 学生信息操作
    bool addStudent(const QString &stuId, const QString &name, const QString &className,
                    double chinese, double math, double english);
    bool updateStudent(const QString &stuId, const QString &name, const QString &className,
                       double chinese, double math, double english);
    bool deleteStudent(const QString &stuId);
    QVector<QMap<QString, QVariant>> getAllStudents();
    QVector<QMap<QString, QVariant>> searchStudents(const QString &keyword);

    // 统计函数
    QVector<QMap<QString, QVariant>> getSubjectStats(const QString &subject);
    QVector<QMap<QString, QVariant>> getClassStats();
    QVector<QMap<QString, QVariant>> getScoreDistribution(const QString &subject);
    QVector<QMap<QString, QVariant>> getTrendData();

    // 工具函数
    QStringList getAllClasses();
    bool isStudentExist(const QString &stuId);

private:
    QSqlDatabase db;
};

#endif // DATABASE_H
