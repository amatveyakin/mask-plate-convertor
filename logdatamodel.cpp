#include <QApplication>
#include <QStyle>

#include "logdatamodel.h"


LogDataModel::LogDataModel(QObject* parentArg)
    : ParentT(parentArg)
    , m_iconSize(16)
{
}

void LogDataModel::setIconSize(int size)
{
    m_iconSize = size;
    if (rowCount() > 0)
        emit dataChanged(index(0), index(rowCount() - 1));
}


void LogDataModel::clear()
{
    beginResetModel();
    m_lines.clear();
    endResetModel();
}

void LogDataModel::addLine(LogDataModel::Icon icon, const QString& text, TextPosition sourceTextPosition)
{
    beginInsertRows(QModelIndex(), m_lines.size(), m_lines.size());
    m_lines.emplace_back(icon, text, sourceTextPosition);
    endInsertRows();
}


int LogDataModel::rowCount(const QModelIndex& parentIndex) const
{
    return parentIndex.isValid() ? 0 : m_lines.size();
}

QVariant LogDataModel::data(const QModelIndex& idx, int role) const
{
    if (idx.column() != 0 || (size_t)idx.row() >= m_lines.size())
        return QVariant();
    const Line& line = m_lines[idx.row()];
    switch (role) {
    case Qt::DisplayRole:
        return line.text;
    case Qt::DecorationRole:
        switch (line.icon) {
        case Neutral: {
            QPixmap emptyPixmap(m_iconSize, m_iconSize);
            emptyPixmap.fill(Qt::transparent);
            return emptyPixmap;
        }
        case Warning:   return QPixmap(":/images/log-warning.png").scaledToHeight(m_iconSize, Qt::SmoothTransformation);
        case Error:     return QPixmap(":/images/log-error.png").scaledToHeight(m_iconSize, Qt::SmoothTransformation);
        }
        return QVariant();
    case SourceTextPositionRole:
        return QVariant::fromValue(line.sourceTextPosition);
    }
    return QVariant();
}
