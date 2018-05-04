#include "planner.h"
#include <QTime>
#include <QDebug>


Planner::Planner(QObject *parent) : QAbstractTableModel(parent)
{
    //qRegisterMetaType<QList<task>>("QList<task>");
    //qRegisterMetaType<task>("task");
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
    if(workContentMap.contains(kanban))
        plan.append({kanban,workContentMap.value(kanban)});
    emit planChanged(getPlan());
}

QList<int> Planner::getPlan()
{
    QList<int> planList;
    int partsCount=0;
    QList<task> planCopy;
    planCopy.append(plan);
    //qDebug()<<"getPlan";
    int hourValue=3600-QTime::currentTime().minute()*60+QTime::currentTime().second();
    while (!planCopy.isEmpty()){
        //qDebug()<<"planCopy.at(0).workContent"<<planCopy.at(0).workContent;
        while (planCopy.at(0).workContent<hourValue){
            hourValue-=planCopy.takeAt(0).workContent;
            partsCount++;
            if(planCopy.isEmpty())
                break;
            //qDebug()<<"partsCount"<<partsCount;
        }
        planList.append(partsCount);
        partsCount=0;
        hourValue=3600;
    }
    //qDebug()<<"planList"<<planList;
    return planList;
}

int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
   return 2;
}

int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 3;
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{

}

