#include <QApplication>
#include <QStyle>

#include "logdatamodel.h"


LogDataModel::LogDataModel(QObject* parentArg)
    : ParentT(parentArg)
    , m_iconSize(16)
{
    QPixmap emptyPixmap(m_iconSize, m_iconSize);
    emptyPixmap.fill(Qt::transparent);
    m_pixmaps[Neutral] = emptyPixmap;
    m_pixmaps[Warning] = QPixmap(":/images/log-warning.png").scaledToHeight(m_iconSize, Qt::SmoothTransformation);
    m_pixmaps[Error] = QPixmap(":/images/log-error.png").scaledToHeight(m_iconSize, Qt::SmoothTransformation);
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

void LogDataModel::addLine(LogDataModel::Severity severity, const QString& text, TextPosition sourceTextPosition)
{
    beginInsertRows(QModelIndex(), m_lines.size(), m_lines.size());
    m_lines.push_back({severity, text, sourceTextPosition});
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
        return m_pixmaps.at(line.severity);
    case SourceTextPositionRole:
        return QVariant::fromValue(line.sourceTextPosition);
    }
    return QVariant();
}
