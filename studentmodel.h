#ifndef STUDENTMODEL_H
#define STUDENTMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QMap>
#include <QVariant>

class StudentModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit StudentModel(QObject *parent = nullptr);

    // 重写基类函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 自定义函数
    void setData(const QVector<QMap<QString, QVariant>> &students);
    QMap<QString, QVariant> getStudent(int row) const;
    void clear();

private:
    QVector<QMap<QString, QVariant>> studentList;
    QStringList headers;
};

#endif // STUDENTMODEL_H
