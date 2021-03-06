/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    delegate.cpp

    A delegate that allows the user to change integer values from the model
    using a spin box widget.
*/

#include <QtGui>
#include <QLineEdit>
#include <QApplication>

#include "SpinBoxDelegate.h"


//! [0]
SpinBoxDelegate::SpinBoxDelegate(QObject *parent, int minVal, int maxVal)
    : QItemDelegate(parent),
    minVal(minVal),
    maxVal(maxVal)
{

}
//! [0]

//! [1]
QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent/*static_cast<QWidget *>(this->parent())*/);

    editor->setObjectName("QSpinBox");
    editor->findChild<QLineEdit*>()->setReadOnly(true);
//    editor->installEventFilter(parent);
    editor->setMinimum(minVal);
    editor->setMaximum(maxVal);

    return editor;
}
//! [1]

//! [2]
void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    const QAbstractItemModel *model = index.model();
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
//    if(index.column()==Planner::Columns::COL_ACTUAL)
//        spinBox->setMaximum(model->data(model->index(index.row(),Planner::Columns::COL_PLAN)).toUInt());//TBD
//    if(index.column()==Planner::Columns::COL_LOSTTIME)
//        spinBox->setMaximum(60);
//    if(index.column()==Planner::Columns::COL_SCRAP)
//        spinBox->setMaximum(model->data(model->index(index.row(),(int)Planner::Columns::COL_ACTUAL)).toUInt());
    spinBox->setValue(value);
}
//! [2]

//! [3]
void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
//! [3]

//! [4]
void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
//    editor->setStyleSheet("QSpinBox::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    editor->setStyleSheet("QSpinBox::up-arrow {  width: 7px;  height: 7px; }"
//                         "QSpinBox::down-arrow {  width: 10px;  height: 7px; }");
//    editor->setStyleSheet("QSpinBox:hover{border-width: 2px;border-style: solid;border-color: blue;}");
//    const QWidget* const widget = option.widget;
//    QStyle* const style = widget ? widget->style() : QApplication::style();
//    QStyleOptionComboBox cbOption;
//    cbOption.rect = option.rect;
//    cbOption.currentText = index.data(Qt::DisplayRole).toString();
//    if(index.flags().testFlag(Qt::ItemIsEditable)){
//           cbOption.direction = option.direction;
//           cbOption.currentIcon = index.data(Qt::DecorationRole).value<QIcon>();
//           cbOption.fontMetrics = option.fontMetrics;
//           const int iconWidth = style->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, widget);
//           cbOption.iconSize = QSize(iconWidth, iconWidth);
//           cbOption.palette = option.palette;
//           cbOption.state = option.state;
//           cbOption.styleObject = option.styleObject;
//    }

//    editor->setProperty("col",index.column());
//    editor->setProperty("row",index.row());
//    editor->setProperty("rect",option.rect);
    editor->setGeometry(option.rect);
}

//void SpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
//{
//    QStyleOptionViewItemV4 myOption = option;
//    myOption.text = Items[index.data().toInt()].c_str();
//    if(index.data().toInt()!=0)
//        qDebug()<<"index.data().toInt()"<<index.data().toInt()<<"myOption.text"<<myOption.text;
//  //  if(!myOption.text.isEmpty())
//  //      qDebug()<<"Items[index.data().toInt()].c_str()"<<myOption.text;
//  //  myOption.text = index.data().toString();
//  //  if(!myOption.text.isEmpty())
//  //      qDebug()<<"index.data().toString()"<<index.data().toString();
//  //  if(index.data().toInt()!=0)
//    //setEditorData(,index);
//  //  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
//  //  if (!m_alwaysPaintCombo)
//  //       return ComboBoxDelegate::paint(painter, option, index);
//     const QWidget* const widget = option.widget;
//     QStyle* const style = widget ? widget->style() : QApplication::style();
//     QStyleOptionComboBox cbOption;
//     cbOption.rect = option.rect;
//     cbOption.currentText = index.data(Qt::DisplayRole).toString();
//     if(index.flags().testFlag(Qt::ItemIsEditable)){
//            cbOption.direction = option.direction;
//            cbOption.currentIcon = index.data(Qt::DecorationRole).value<QIcon>();
//            cbOption.fontMetrics = option.fontMetrics;
//            const int iconWidth = style->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, widget);
//            cbOption.iconSize = QSize(iconWidth, iconWidth);
//            cbOption.palette = option.palette;
//            cbOption.state = option.state;
//            cbOption.styleObject = option.styleObject;
//     }
//     style->drawComplexControl(QStyle::CC_ComboBox, &cbOption, painter, widget);
//     style->drawControl(QStyle::CE_ComboBoxLabel, &cbOption, painter, widget);
//}

//virtual void	paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {}

//void SpinBoxDelegate::paintEvent(QPaintEvent *e) {
//   QSpinBox::paintEvent(e);
//   if (model() && model()->rowCount(rootIndex()) > 0) return;
//   // The view is empty.
//   QPainter p(this->viewport());
//   p.drawText(rect(0,0,0), Qt::AlignCenter, "");
//}

//! [4]
//!
//    bool SpinBoxDelegate::eventFilter(QObject *o, QEvent *e)
//{
//    if (e->type() == QEvent::KeyPress) {
//        e->ignore();
//        return true;
//    }
//    return SpinBoxDelegate::eventFilter(o, e);
//}


void SpinBoxDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
    painter->setPen(QColor(Qt::black));
    painter->drawRect(option.rect);
    painter->restore();
    QItemDelegate::paint(painter, option, index);
}

