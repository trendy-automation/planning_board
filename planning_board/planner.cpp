#include "planner.h"
#include <QDate>
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
        hourTimer->start(qMax(3600000-2000,ct.msecsTo(QTime(ct.hour(),59,59))+2000));
        int hourNumber = getCurrentHourNum();
        if(hourNumber!=0 && (ct.hour()==0||ct.hour()==6||ct.hour()==15))
            qDebug()<<"Timer Error!!!!!!!!!!! Warning!!!"<<"hourNumber"<<hourNumber<<"ct.hour()"<<ct.hour();
        //qDebug()<<1<<planBoard.count()<<"hourNumber"<<hourNumber;
        //auto done task huck
        int doneHour=hourNumber==0?(planBoard.count()-1):(hourNumber-1);
        //qDebug()<<"hourNumber"<<hourNumber<<"doneHour"<<doneHour;
        for(int i=0;i<planBoard.at(doneHour)->hourPlan.count();++i)
            planBoard.at(doneHour)->hourPlan.at(i)->setDone();
        //qDebug()<<"hourNumber"<<hourNumber;
        if(hourNumber==0){
            //save excel report
            saveExcelReport(QDate().currentDate().toString(
                                QString("BoardReport_yyyy_MMMM_dd_%1.xlsx").arg(ct.hour())));
            planBoard.clear();
        }
        this->planBoardUpdate();
    });
    QTime ct = QTime::currentTime();
    hourTimer->start(ct.msecsTo(QTime(ct.hour(),59,59))+2000);
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
    setProperty("lostTimeNoteList",lostTimeNoteList);
    setProperty("scrapNoteList",scrapNoteList);
}

bool Planner::readExcelData(const QString &fileName)
{
    kanbanMap.clear();
    Document *xlsx= new Document(fileName);
    //xlsx->saveAs(fileName);
    bool ok=xlsx->selectSheet("Parts");
    int i=2;
    QByteArray kanban;
    if(ok)
        kanban = xlsx->read(i,1).toByteArray();
    while (ok && !kanban.isEmpty()){
        int partWorkContent = xlsx->read(i,2).toInt();
        if(partWorkContent<3600){
            kanbanItem kanbanObj;
            kanbanObj.kanban=kanban;
            kanbanObj.partWorkContent=partWorkContent;
            kanbanObj.countParts=xlsx->read(i,3).toInt();
            kanbanObj.reference=xlsx->read(i,4).toByteArray();
            kanbanObj.sebango=xlsx->read(i,5).toByteArray();
            kanbanMap.insert(kanban,kanbanObj);
            //            qDebug()<<kanbanObj.kanban<<kanbanObj.partWorkContent
            //                    <<kanbanObj.countParts<<kanbanObj.reference<<kanbanObj.sebango;
        }
        i++;
        kanban = xlsx->read(i,1).toByteArray();
    }
    ok&=xlsx->selectSheet("Notes");
    i=2;
    QByteArray note;
    if(ok)
        note = xlsx->read(i,1).toByteArray();
    lostTimeNoteList.append("");
    while (ok && !note.isEmpty()){
        lostTimeNoteList.append(note);
        i++;
        note = xlsx->read(i,1).toByteArray();
    }
    return ok;
}

void Planner::addKanban(const QByteArray &kanban)
{
    if(kanbanMap.contains(kanban)){
        taskItem *task = new taskItem(kanbanMap.value(kanban));
        task->taskWorkContent=task->kanbanObj.countParts*task->kanbanObj.partWorkContent;
        notAttachedTasks.append(task);
        this->planBoardUpdate();
        return;
    }
    if(kanban=="34703470"){
        qDebug()<<"refresh";
        readExcelData();
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
    int hour=hourNumber;
    //qDebug()<<"hour"<<hour<<"hourValue1"<<hourValue;
    while(hour<planBoard.count() && notAttachedTasks.count()>0){
        for(int i=0;i<planBoard.at(hour)->hourPlan.count();++i)
            if(!planBoard.at(hour)->hourPlan.at(i)->done)
                hourValue-=planBoard.at(hour)->hourPlan.at(i)->taskWorkContent;
        //qDebug()<<"hour"<<hour<<"hourValue2"<<hourValue;
        for(int i=0;i<notAttachedTasks.count();++i){
            if(notAttachedTasks.at(0)->taskWorkContent<hourValue){
                //qDebug()<<"hour"<<hour<<"hourValue3"<<hourValue<<"tasks.count()"<<tasks.count();
                hourValue-=notAttachedTasks.at(0)->taskWorkContent;
                //qDebug()<<"hour"<<hour<<"hourValue4"<<hourValue<<"tasks.at(0)->taskWorkContent"<<tasks.at(0)->taskWorkContent;
                planBoard.at(hour)->appendTask(notAttachedTasks.takeAt(0));
                // // // //if(planBoard.at(hour)->hourPlan.count()>1){
                //insertRow(hour);
                //emit modelSpanned(int,int,int,int);
                //}
            }
        }
        hour++;
        hourValue=3600;
    }

    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomRight = createIndex(rowCount(),columnCount());
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, bottomRight);
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
        return QFont("Arial",FONT_VALUE);
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
    return planBoard.count();
}

int Planner::columnCount(const QModelIndex & /*parent*/) const
{
    return headers.count();
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
                //if(!planBoard.at(row)->hourPlan.at(i)->done)
                plan+=planBoard.at(row)->hourPlan.at(i)->kanbanObj.countParts;
            return plan;
        }
        case Columns::COL_ACTUAL:{
            int act=0;
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                if(planBoard.at(row)->hourPlan.at(i)->done)
                    act+=planBoard.at(row)->hourPlan.at(i)->kanbanObj.countParts;
            return act;
        }
        case Columns::COL_REFERENCE:{
            QString ref;
            QMap<QByteArray,int> references;
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                if(references.contains(planBoard.at(row)->hourPlan.at(i)->kanbanObj.reference))
                    references.insert(planBoard.at(row)->hourPlan.at(i)->kanbanObj.reference,
                                       references.value(planBoard.at(row)->hourPlan.at(i)->kanbanObj.reference)+
                                        planBoard.at(row)->hourPlan.at(i)->kanbanObj.countParts);
                else
                    references.insert(planBoard.at(row)->hourPlan.at(i)->kanbanObj.reference,
                                       planBoard.at(row)->hourPlan.at(i)->kanbanObj.countParts);
            for(int i=0;i<references.count();++i)
                if(references.value(references.keys().at(i))>1)
                    ref.append(references.keys().at(i)).append(" x ")
                            .append(QString::number(references.value(references.keys().at(i)))).append("\n");
                else
                    ref.append(references.keys().at(i)).append("\n");
            return ref;
        }
        case Columns::COL_LOSTTIME:
            return planBoard.at(row)->lostTime;
        case Columns::COL_NOTES:{
            QString noteStr;
            if(!planBoard.at(row)->lostTimeNote.isEmpty())
                noteStr=planBoard.at(row)->lostTimeNote;
            return noteStr;
        }
        case Columns::COL_SCRAP:{
            int scrap=0;
            for(int i=0;i<planBoard.at(row)->hourPlan.count();++i)
                scrap+=planBoard.at(row)->hourPlan.at(i)->countScrap;
            return scrap;
        }
        default:
            return QVariant();
        }
        break;
    }
    case Qt::FontRole:
        return QFont("Helvetica [Croyx]",FONT_VALUE);
        break;
    case Qt::BackgroundRole:
        //QBrush redBackground(Qt::red);
        //return redBackground;
        return QVariant();
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    case Qt::CheckStateRole:
        //return Qt::Checked;
        return QVariant();
        break;
    case Qt::ForegroundRole:{
        return QVariant();
        break;
    }
    case Qt::ToolTipRole:{
        return QVariant();
        break;
    }
    case Qt::EditRole:{
        switch(col){
        case Columns::COL_PERIOD:
            return data(this->index(row,col),Qt::DisplayRole);
        case Columns::COL_PLAN:
            return data(this->index(row,col),Qt::DisplayRole);
        case Columns::COL_ACTUAL:
            return data(this->index(row,col),Qt::DisplayRole);
        case Columns::COL_REFERENCE:
            return data(this->index(row,col),Qt::DisplayRole);
        case Columns::COL_LOSTTIME:
            return data(this->index(row,col),Qt::DisplayRole);
        case Columns::COL_NOTES:{
            int note=0;
            if(!planBoard.at(row)->lostTimeNote.isEmpty())
                note=lostTimeNoteList.indexOf(planBoard.at(row)->lostTimeNote);
            return note;
        }
        case Columns::COL_SCRAP:
            return data(this->index(row,col),Qt::DisplayRole);
            break;
        }
    }
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
            planBoard.at(index.row())->lostTimeNote=lostTimeNoteList.at(value.toInt());
            break;
        case Columns::COL_SCRAP:
            if(!planBoard.at(index.row())->hourPlan.isEmpty())
                planBoard.at(index.row())->hourPlan.at(0)->countScrap=value.toInt();

            //huck
        }
        emit editCompleted( value.toString() );
    }
    return true;
}

Qt::ItemFlags Planner::flags(const QModelIndex & index) const
{
    int hourNumber;
    QTime ct=QTime::currentTime();
    if(ct.hour()<6)
        hourNumber=ct.hour();
    else if (ct.hour()<15)
        hourNumber=ct.hour()-6;
    else
        hourNumber=ct.hour()-15;
    if(index.row()<=hourNumber){
        switch(index.column()){
        //        case Columns::COL_ACTUAL:
        //            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        case Columns::COL_LOSTTIME:
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        case Columns::COL_SCRAP:
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        case Columns::COL_NOTES:
            //            if(planBoard.at(index.row())->lostTimeNotes.isEmpty()||
            //                    planBoard.at(index.row())->lostTimeNotes=="0")
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        }
    }
    return QAbstractTableModel::flags(index) & ~Qt::ItemIsEditable;
}


bool Planner::saveExcelReport(const QString &fileName)
{
    qDebug()<<"saveExcelReport"<<"fileName";
    Document *xlsx= new Document(fileName);
    for(int j=0;j<headers.count();j++)
        xlsx->write(1,j+1,headers.value(headers.keys().at(j)));
    for(int j=0;j<headers.count();j++)
        for(int i=0;i<planBoard.count();i++)
            if(j==Columns::COL_NOTES)
                xlsx->write(i+2,j+1,lostTimeNoteList.at(data(index(i,j)).toInt()));
            else
                xlsx->write(i+2,j+1,data(index(i,j)));
    return xlsx->saveAs(fileName);
}

int Planner::getProgress()
{
    int workContent=0;
    int doneContent=0;
    for(int i=0;i<planBoard.count();i++)
        for(int j=0;j<planBoard.at(i)->hourPlan.count();j++){
            workContent+=planBoard.at(i)->hourPlan.at(j)->taskWorkContent;
            if(planBoard.at(i)->hourPlan.at(j)->done)
                doneContent+=planBoard.at(i)->hourPlan.at(j)->taskWorkContent;
            //            qDebug()<<doneContent<<workContent;
        }
    //    qDebug()<<doneContent<<workContent;
    if(workContent==0)
        return 0;
    return doneContent*100/workContent;
}

