#ifndef LOGDATAMODEL_H
#define LOGDATAMODEL_H

#include <QAbstractItemModel>
#include <QPixmap>

#include "textbasic.h"


class LogDataModel : public QAbstractListModel
{
    Q_OBJECT
    typedef QAbstractListModel ParentT;

public:
    enum Icon
    {
        Neutral,
        Warning,
        Error
    };

    enum Roles
    {
        SourceTextPositionRole = Qt::UserRole,
    };

public:
    explicit LogDataModel(QObject* parentArg = 0);

    void setIconSize(int size);

    void clear();
    void addLine(Icon icon, const QString& text, TextPosition sourceTextPosition);

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex& parentIndex = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& idx, int role) const;

private:
    struct Line
    {
        Icon icon;
        QString text;
        TextPosition sourceTextPosition;    ///< corresponding source text position that will be focused on clicked; invalid - no position
    };

private:
    int m_iconSize = 0;
    std::vector<Line> m_lines;
    std::map<Icon, QPixmap> m_pixmaps;
};

#endif // LOGDATAMODEL_H
