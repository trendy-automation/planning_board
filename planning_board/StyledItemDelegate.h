#ifndef StyledItemDelegate_H
#define StyledItemDelegate_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>
#include <QApplication>

class StyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    StyledItemDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

//    QSize sizeHint(const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QSize baseSize = this->QStyledItemDelegate::sizeHint(option, index);
        baseSize.setHeight(10000);//something very high, or the maximum height of your text block
//        qDebug() << "index.data()" << index.data().toString();
        QFontMetrics metrics(option.font);
        QString text(index.data().toString());
        QRect outRect = metrics.boundingRect(QRect(QPoint(0, 0), baseSize), Qt::AlignCenter, text);
//        qDebug() << outRect.height();
        //baseSize.setHeight(outRect.height()*(text.count("\n")-1));
        //baseSize.setHeight(qMax(qMax(58,outRect.height()),24*(text.count("\n"))));
        baseSize.setHeight(qMax(qApp->property("minRowHeight").toInt(),outRect.height()));
        //if (text.count("\n")<1)
        //    baseSize.setHeight(outRect.height());
        //else
        //    baseSize.setHeight(outRect.height()/2*(text.count("\n")+1));
        //if(baseSize.height()>58)
        //    qDebug()<<outRect.height()<<text.count("\n")<<baseSize.height()<<text;
        //baseSize.setHeight(qMax(58,outRect.height()));
        return baseSize;
    }


    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        painter->save();
        painter->setPen(QColor(Qt::black));
        painter->drawRect(option.rect);
        painter->restore();
        QStyledItemDelegate::paint(painter, option, index);
    }

};

#endif
