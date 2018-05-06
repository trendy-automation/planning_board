#include "planner.h"
#include <QTime>
#include <QDebug>


Planner::Planner(QObject *parent) : QAbstractTableModel(parent)
{
    //qRegisterMetaType<QList<task>>("QList<task>");
    //qRegisterMetaType<task>("task");
//    setHeaderData(0, Qt::Horizontal, tr("Период"));
//    setHeaderData(1, Qt::Horizontal, tr("План"));

    //    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    //    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
        ComboBoxDelegate *comboDelegate = new ComboBoxDelegate(this);
        //QAbstractItemView::setItemDelegateForColumn();

        //setItemDelegateForColumn();
//        this->setItemData()
//        tableWidget->setItemDelegateForColumn(3,comboDelegate);


}

bool Planner::readExcelData(const QString &fileName)
{
    workContentMap.clear();
    Document *xlsx= new Document(fileName);
    xlsx->saveAs(fileName);
    QByteArray kanban;
    int workContent;
    int i=2;
    kanban=xlsx->read(i,1).toByteArray();
    workContent=xlsx->read(i,2).toInt();
    while (!kanban.isEmpty()){
        i++;
        //qDebug()<<"kanban"<<kanban<<"workContent"<<workContent;
        if(workContent<3600)
            workContentMap.insert(kanban,workContent);
        kanban=xlsx->read(i,1).toByteArray();
        workContent=xlsx->read(i,2).toInt();
    }

    return true;
}

void Planner::addPlan(const QByteArray &kanban)
{
    //qDebug()<<kanban<<workContentMap;
//    if(workContentMap.contains(kanban))
//        plan.append({kanban,workContentMap.value(kanban)});
    //emit planChanged(getPlan());
}

//QList<int> Planner::getPlan()
//{
//    QList<int> planList;
//    int partsCount=0;
//    QList<task> planCopy;
//    planCopy.append(plan);
//    //qDebug()<<"getPlan";
//    int hourValue=3600-QTime::currentTime().minute()*60+QTime::currentTime().second();
//    while (!planCopy.isEmpty()){
//        //qDebug()<<"planCopy.at(0).workContent"<<planCopy.at(0).workContent;
//        while (planCopy.at(0).workContent<hourValue){
//            hourValue-=planCopy.takeAt(0).workContent;
//            partsCount++;
//            if(planCopy.isEmpty())
//                break;
//            //qDebug()<<"partsCount"<<partsCount;
//        }
//        planList.append(partsCount);
//        partsCount=0;
//        hourValue=3600;
//    }
//    //qDebug()<<"planList"<<planList;
//    return planList;
//}
QVariant Planner::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("first");
            case 1:
                return QString("second");
            case 2:
                return QString("third");
            }
        }
    }
    return QVariant();
}

int Planner::rowCount(const QModelIndex & /*parent*/) const
{
   return (QTime::currentTime().hour()<6)?6:9;
}

int Planner::columnCount(const QModelIndex & /*parent*/) const
{
    return 7;
}

QVariant Planner::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole)
    {
        if (row == 0 && col == 0)
        {
            return QTime::currentTime().toString();
        }
    }
    /*
    switch(role){
    case Qt::DisplayRole:
        if (row == 0 && col == 1) return QString("<--left");
        if (row == 1 && col == 1) return QString("right-->");

        return QString("Row%1, Column%2")
                .arg(row + 1)
                .arg(col +1);
        break;
    case Qt::FontRole:
        if (row == 0 && col == 0) //change font only for cell(0,0)
        {
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
        break;
    case Qt::BackgroundRole:

        if (row == 1 && col == 2)  //change background only for cell(1,2)
        {
            QBrush redBackground(Qt::red);
            return redBackground;
        }
        break;
    case Qt::TextAlignmentRole:

        if (row == 1 && col == 1) //change text alignment only for cell(1,1)
        {
            return Qt::AlignRight + Qt::AlignVCenter;
        }
        break;
    case Qt::CheckStateRole:

        if (row == 1 && col == 0) //add a checkbox to cell(1,0)
        {
            return Qt::Checked;
        }
    }
*/
    return QVariant();
}

bool Planner::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
      {
          //save value from editor to member m_gridData
          m_gridData[index.row()][index.column()] = value.toString();
          //for presentation purposes only: build and emit a joined string
          QString result;
          for (int row= 0; row < 9; row++)
          {
              for(int col= 0; col < 7; col++)
              {
                  result += m_gridData[row][col] + ' ';
              }
          }
          emit editCompleted( result );
      }
      return true;
}

Qt::ItemFlags Planner::flags(const QModelIndex & index) const
{
   return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void Planner::timerHit()
{
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
}
