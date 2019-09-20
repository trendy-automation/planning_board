#ifndef GRIDDELEGATE_H
#define GRIDDELEGATE_H

#include <QStyledItemDelegate>
#include <QDebug>
#include <QPainter>

class GridDelegate : public QStyledItemDelegate
{
public:
    explicit GridDelegate(QObject * parent = 0) : QStyledItemDelegate(parent) { }

    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        painter->save();
        painter->setPen(QColor(Qt::black));
        painter->drawRect(option.rect);
        painter->restore();
        QStyledItemDelegate::paint(painter, option, index);
    }
};

#endif // GRIDDELEGATE_H
