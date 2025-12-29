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

private:
    QSqlDatabase db;
};

#endif // DATABASE_H

bool addStudent(const QString &stuId, const QString &name, const QString &className);
