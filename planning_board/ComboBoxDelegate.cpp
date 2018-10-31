#include "ComboBoxDelegate.h"

#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QTreeView>
#include <QStringList>
#include <QString>

#include <iostream>

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
:QItemDelegate(parent)
{
    QTreeView *treeView = static_cast<QTreeView*>(parent);
    QStringList lostTimeNoteList = treeView->model()->property("lostTimeNoteList").toStringList();
    //QStringList scrapNoteList = treeView->model()->property("scrapNoteList").toStringList(); //TBD

    for(int i=0;i<lostTimeNoteList.count();++i)
        Items.push_back(lostTimeNoteList.at(i).toStdString());
//    for(int i=0;i<scrapNoteList.count();++i)
//        Items.push_back(scrapNoteList.at(i).toStdString());

}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
  QComboBox* editor = new QComboBox(parent);
  editor->setFont(parent->font());
  for(unsigned int i = 0; i < Items.size(); ++i)
    {
    editor->addItem(Items[i].c_str());
    }
  return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  int value = index.model()->data(index, Qt::EditRole).toInt();
//  qDebug()<<"index.data()"<<index.data()<<"index.data(Qt::EditRole)"<<index.data(Qt::EditRole);
  comboBox->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
//  model->setData(index, Items[comboBox->currentIndex()].c_str(), Qt::DisplayRole);
  model->setData(index, comboBox->currentIndex(), Qt::EditRole);
//  qDebug()<<"index.data()"<<index.data()<<"index.data(Qt::EditRole)"<<index.data(Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
//  editor->setStyleSheet("QComboBox::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
  editor->setGeometry(option.rect);
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItemV4 myOption = option;
  myOption.text = Items[index.data().toInt()].c_str();
  if(index.data().toInt()!=0)
      qDebug()<<"index.data().toInt()"<<index.data().toInt()<<"myOption.text"<<myOption.text;
//  if(!myOption.text.isEmpty())
//      qDebug()<<"Items[index.data().toInt()].c_str()"<<myOption.text;
//  myOption.text = index.data().toString();
//  if(!myOption.text.isEmpty())
//      qDebug()<<"index.data().toString()"<<index.data().toString();
//  if(index.data().toInt()!=0)
  //setEditorData(,index);
//  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
//  if (!m_alwaysPaintCombo)
//       return ComboBoxDelegate::paint(painter, option, index);
   const QWidget* const widget = option.widget;
   QStyle* const style = widget ? widget->style() : QApplication::style();
   QStyleOptionComboBox cbOption;
   cbOption.rect = option.rect;
   cbOption.currentText = index.data(Qt::DisplayRole).toString();
   if(index.flags().testFlag(Qt::ItemIsEditable)){
          cbOption.direction = option.direction;
          cbOption.currentIcon = index.data(Qt::DecorationRole).value<QIcon>();
          cbOption.fontMetrics = option.fontMetrics;
          const int iconWidth = style->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, widget);
          cbOption.iconSize = QSize(iconWidth, iconWidth);
          cbOption.palette = option.palette;
          cbOption.state = option.state;
          cbOption.styleObject = option.styleObject;
   }
   style->drawComplexControl(QStyle::CC_ComboBox, &cbOption, painter, widget);
   style->drawControl(QStyle::CE_ComboBoxLabel, &cbOption, painter, widget);


}
