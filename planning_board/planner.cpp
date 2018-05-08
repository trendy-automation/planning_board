#include "planner.h"
#include <QTime>
#include <QTimer>
#include <QDebug>

Planner::Planner(QObject *parent) : QAbstractTableModel(parent)
{
    headers.insert(COL_PERIOD,"Период");
    headers.insert(COL_PLAN,"План");
    headers.insert(COL_ACTUAL,"Факт");
    headers.insert(COL_REFERENCE,"Референс");
    headers.insert(COL_LOSTTIME,"Потерянное\nвремя");
    headers.insert(COL_SCRAP,"Брак");
    headers.insert(COL_NOTES,"Замечания");
    planBoardUpdate();
    this->readExcelData();
    QTimer *hourTimer = new QTimer(this);
    QObject::connect(hourTimer,&QTimer::timeout,[hourTimer,this](){
        QTime ct = QTime::currentTime();
        hourTimer->start(qMax(3600000-2000,ct.msecsTo(QTime(ct.hour(),59,59))+1000));
        //auto done task huck
        int hourNumber = getCurrentHourNum();
        for(int i=0;i<planBoard.at(hourNumber)->hourPlan.count();++i)
            planBoard.at(hourNumber)->hourPlan.at(i)->setDone();
        //save excel report
        if(ct.hour()==0||ct.hour()==6||ct.hour()==15)
            planBoard.clear();
        this->planBoardUpdate();
    });
    QTime ct = QTime::currentTime();
    hourTimer->start(ct.msecsTo(QTime(ct.hour(),59,59)));
}

bool Planner::readExcelData(const QString &fileName)
{
    kanbanMap.clear();
    Document *xlsx= new Document(fileName);
    xlsx->saveAs(fileName);
    int i=2;
    QByteArray kanban = xlsx->read(i,1).toByteArray();
    while (!kanban.isEmpty()){
        kanban = xlsx->read(i,1).toByteArray();
        int partWorkContent = xlsx->read(i,2).toInt();
        if(partWorkContent<3600){
            kanbanItem kanbanObj;
            kanbanObj.kanban=xlsx->read(i,1).toByteArray();
            kanbanObj.partWorkContent=partWorkContent;
            kanbanObj.countParts=xlsx->read(i,3).toInt();
            kanbanObj.reference=xlsx->read(i,4).toByteArray();
            kanbanObj.sebango=xlsx->read(i,5).toByteArray();
            kanbanMap.insert(kanban,kanbanObj);
            //            qDebug()<<kanbanObj.kanban<<kanbanObj.partWorkContent
            //                    <<kanbanObj.countParts<<kanbanObj.reference<<kanbanObj.sebango;
        }
        i++;
    }
    return true;
}

void Planner::addKanban(const QByteArray &kanban)
{
    if(kanbanMap.contains(kanban)){
        taskItem *task = new taskItem(kanbanMap.value(kanban));
        task->taskWorkContent=task->kanbanObj.countParts*task->kanbanObj.partWorkContent;
        tasks.append(task);
        this->planBoardUpdate();
    }
}

void Planner::planBoardUpdate()
{
    QTime ct=QTime::currentTime();
    int hourValue=3600-ct.minute()*60+ct.second();
    int hourNumber=getCurrentHourNum();
    int hoursCount=(QTime::currentTime().hour()<6)?6:9;
    if(planBoard.count()!=hoursCount){
        planBoard.clear();
        for(int i=0;i<hoursCount;++i)
            planBoard.append(new hourItem());
    }
    for(int i=0;i<planBoard.count();++i)
        if(!planBoard.at(i)){
            qDebug()<<"!planBoard.at(i)";
            planBoard.insert(i,new hourItem());
        }
    for(int hour=hourNumber;hour<planBoard.count();++hour){
        for(int i=0;i<planBoard.at(hour)->hourPlan.count();++i)
            if(!planBoard.at(hour)->hourPlan.at(i)->done)
                hourValue=-planBoard.at(hour)->hourPlan.at(i)->taskWorkContent;
        for(int i=0;i<tasks.count();++i){
            if(tasks.at(i)->taskWorkContent<hourValue){
                hourValue=-tasks.at(i)->taskWorkContent;
                planBoard.at(hour)->appendTask(tasks.takeAt(i));
            }
        }
        hourValue=3600;
    }

    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
}

int Planner::getCurrentHourNum()
{
    QTime ct=QTime::currentTime();
    int hourNumber;
    if(ct.hour()<6)
        hourNumber=ct.hour();
    else if (ct.hour()<15)
        hourNumber=ct.hour()-6;
    else
        hourNumber=ct.hour()-15;
    return hourNumber;
}

QVariant Planner::headerData(int section, Qt::Orientation orientation, int role) const
{

    switch(role){
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal) {
            if(section>=0 && section<headers.count())
                return headers.value((Columns)section);
        }
        break;
    case Qt::FontRole:
        return QFont("Arial",16);
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    case Qt::SizeHintRole:
        if (orientation == Qt::Horizontal) {
//                        switch(section){
//                        case Columns::COL_PERIOD:
//                            return QSize(250,70);
//                        case Columns::COL_PLAN:
//                            return QSize(50,70);
//                        case Columns::COL_ACTUAL:
//                            return QSize(50,70);
//                        case Columns::COL_REFERENCE:
//                            return QSize(150,70);
//                        case Columns::COL_LOSTTIME:
//                            return QSize(50,70);
//                        case Columns::COL_NOTES:
//                            return QSize(300,70);
//                        case Columns::COL_SCRAP:
//                            return QSize(50,70);
//                        }
        }
        break;
    }
    return QVariant();
}

int Planner::rowCount(const QModelIndex & /*parent*/) const
{
    //return (QTime::currentTime().hour()<6)?6:9;
    return planBoard.size();
}

int Planner::columnCount(const QModelIndex & /*parent*/) const
{
    return headers.size();
}

QVariant Planner::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    switch(role){
    case Qt::DisplayRole: {
        int startHour;
        QTime ct=QTime::currentTime();
        if(ct.hour()<6)
            startHour=0;
        else if (ct.hour()<15)
            startHour=6;
        else
            startHour=15;
        switch(col){
        case Columns::COL_PERIOD:
            return QString("%1.00-\n%2.00").arg(startHour+row).arg((startHour+row+1)%24);
        case Columns::COL_PLAN:{
            int plan=0;
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                if(!planBoard.at(row)->hourPlan.at(i)->done)
                    plan=+planBoard.at(row)->hourPlan.at(i)->kanbanObj.countParts;
            return plan;
        }
        case Columns::COL_ACTUAL:{
            int act=0;
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                if(planBoard.at(row)->hourPlan.at(i)->done)
                    act=+planBoard.at(row)->hourPlan.at(i)->kanbanObj.countParts;
            return act;
        }
        case Columns::COL_REFERENCE:{
            QString ref="";
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                ref.append(planBoard.at(row)->hourPlan.at(i)->kanbanObj.reference).append("\n");
            return ref;
        }
        case Columns::COL_LOSTTIME:
            return planBoard.at(row)->lostTime;
        case Columns::COL_NOTES:{
            QString notes="";
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                notes.append(planBoard.at(row)->hourPlan.at(i)->scrapNotes).append("\n");
            if(!planBoard.at(row)->lostTimeNotes.isEmpty())
                notes.append(planBoard.at(row)->lostTimeNotes).append("\n");
            return notes;
        }
        case Columns::COL_SCRAP:{
            int scrap=0;
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                scrap=+planBoard.at(row)->hourPlan.at(i)->countScrap;
            return scrap;
        }
        default:
            return QVariant();
        }
        break;
    }
    case Qt::FontRole:
        return QFont("Helvetica [Croyx]",18);
        break;
    case Qt::BackgroundRole:
        //QBrush redBackground(Qt::red);
        //return redBackground;
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    case Qt::CheckStateRole:
        //return Qt::Checked;
        break;
    }
    return QVariant();
}

bool Planner::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        switch(index.column()){
        case Columns::COL_PERIOD:
            break;
        case Columns::COL_PLAN:
            break;
        case Columns::COL_ACTUAL:
            //TBD
            //planBoard.at(index.row())->hourPlan.at(0)->done=true;
            //value.toInt();
            break;
        case Columns::COL_REFERENCE:
            break;
        case Columns::COL_LOSTTIME:
            planBoard.at(index.row())->lostTime=value.toInt();
            break;
        case Columns::COL_NOTES:
//            if(!planBoard.at(index.row())->lostTimeNotes.isEmpty() &&
//                    value.toString()=="0")
//                return false;
            planBoard.at(index.row())->lostTimeNotes=value.toString();
            break;
        case Columns::COL_SCRAP:
            planBoard.at(index.row())->hourPlan.at(0)->countScrap=value.toInt();
            //huck
        }
        emit editCompleted( value.toString() );
    }
    return true;
}

Qt::ItemFlags Planner::flags(const QModelIndex & index) const
{
    if(index.column()==Columns::COL_NOTES){
        int hourNumber;
        QTime ct=QTime::currentTime();
        if(ct.hour()<6)
            hourNumber=ct.hour();
        else if (ct.hour()<15)
            hourNumber=ct.hour()-6;
        else
            hourNumber=ct.hour()-15;
        if(index.row()<=hourNumber)
            if(planBoard.at(index.row())->lostTimeNotes.isEmpty()||
                    planBoard.at(index.row())->lostTimeNotes=="0")
                return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

    return QAbstractTableModel::flags(index) & ~Qt::ItemIsEditable;
}
