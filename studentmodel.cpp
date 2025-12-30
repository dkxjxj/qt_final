#include "studentmodel.h"
#include <QBrush>
#include <QColor>

StudentModel::StudentModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // 设置表头 - 包含成绩字段
    headers << "学号" << "姓名" << "班级" << "语文" << "数学" << "英语" << "总分";
}

int StudentModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return studentList.size();
}

int StudentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headers.size();
}

QVariant StudentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= studentList.size())
        return QVariant();

    const QMap<QString, QVariant> &student = studentList.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return student["stu_id"];
        case 1: return student["name"];
        case 2: return student["class"];
        case 3: return student["chinese"].isNull() ? "未录入" : student["chinese"];
        case 4: return student["math"].isNull() ? "未录入" : student["math"];
        case 5: return student["english"].isNull() ? "未录入" : student["english"];
        case 6: return student["total"];
        default: return QVariant();
        }
    }
    else if (role == Qt::EditRole) {
        // 编辑模式下返回原始数据
        switch (index.column()) {
        case 0: return student["stu_id"];
        case 1: return student["name"];
        case 2: return student["class"];
        case 3: return student["chinese"];
        case 4: return student["math"];
        case 5: return student["english"];
        case 6: return student["total"];
        default: return QVariant();
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    else if (role == Qt::ForegroundRole) {
        // 成绩颜色标记（不及格显示红色，优秀显示绿色）
        if (index.column() >= 3 && index.column() <= 5) {
            QVariant score = student[headers[index.column()].toLower()];
            if (!score.isNull()) {
                double value = score.toDouble();
                if (value >= 90) {
                    return QBrush(QColor(0, 128, 0));    // 绿色 - 优秀
                } else if (value >= 60) {
                    return QBrush(QColor(0, 0, 0));      // 黑色 - 及格
                } else {
                    return QBrush(QColor(255, 0, 0));    // 红色 - 不及格
                }
            }
        }
    }

    return QVariant();
}

QVariant StudentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section < headers.size())
            return headers[section];
    }

    return QVariant();
}

void StudentModel::setData(const QVector<QMap<QString, QVariant>> &students)
{
    beginResetModel();
    studentList = students;
    endResetModel();
}

QMap<QString, QVariant> StudentModel::getStudent(int row) const
{
    if (row >= 0 && row < studentList.size()) {
        return studentList.at(row);
    }
    return QMap<QString, QVariant>();
}

void StudentModel::clear()
{
    beginResetModel();
    studentList.clear();
    endResetModel();
}
