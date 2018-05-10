#include "ComboBoxDelegate.h"

#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QTableView>
#include <QStringList>
#include <QString>

#include <iostream>

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
:QItemDelegate(parent)
{
    QTableView *tableView = static_cast<QTableView*>(parent);
    QStringList lostTimeNoteList = tableView->model()->property("lostTimeNoteList").toStringList();
    //QStringList scrapNoteList = tableView->model()->property("scrapNoteList").toStringList(); //TBD

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
  comboBox->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItemV4 myOption = option;

  myOption.text = Items[index.data().toInt()].c_str();

  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}
