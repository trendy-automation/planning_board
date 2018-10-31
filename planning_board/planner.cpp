#include "planner.h"
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QDebug>

#include <QDir>

#include <QFileInfo>
#include <QDir>
#include <algorithm>
#include <QFileIconProvider>
#include <QDateTime>
#include <QDebug>
#include <QtConcurrentFilter>


Planner::Planner(QObject *parent) : QAbstractItemModel(parent)
    //,_metaProvider(new QFileIconProvider())

{
    headers.insert(COL_PERIOD,"Период");
    headers.insert(COL_PLAN,"План");
    headers.insert(COL_ACTUAL,"Факт");
    headers.insert(COL_REFERENCE,"Референс");
    headers.insert(COL_LOSTTIME,"Потерянное\nвремя");
    headers.insert(COL_SCRAP,"Брак");
    headers.insert(COL_NOTES,"Замечания");
    headers.insert(COL_OPERATORS,"Количество\nоператоров");
    planBoardUpdate();
    this->readExcelData();
    QTimer *hourTimer = new QTimer(this);
    QObject::connect(hourTimer,&QTimer::timeout,[hourTimer,this](){
        QTime ct = QTime::currentTime();
        hourTimer->start(qMax(3600000-2000,ct.msecsTo(QTime(ct.hour(),59,59))+2000));
        int hourNumber = getCurrentHourNum();
        if(hourNumber!=0 && (ct.hour()==0||ct.hour()==6||ct.hour()==15))
            qDebug()<<"Timer Error!!!!!!!!!!! Warning!!!"<<"hourNumber"<<hourNumber<<"ct.hour()"<<ct.hour();
        //qDebug()<<1<<_tasks.count()<<"hourNumber"<<hourNumber;
//        //auto done task huck
//        int doneHour=hourNumber==0?(_tasks.count()-1):(hourNumber-1);
//        //qDebug()<<"hourNumber"<<hourNumber<<"doneHour"<<doneHour;
//        for(int i=0;i<_tasks.at(doneHour)->children.count();++i)
//            _tasks.at(doneHour)->children.at(i).setDone();
        //qDebug()<<"hourNumber"<<hourNumber;
        if(hourNumber==0){
            //save excel report
            saveExcelReport(QDate().currentDate().toString(
                                QString("BoardReport_yyyy_MMMM_dd_%1.xlsx").arg(ct.hour())));
            _tasks.clear();
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

struct Planner::TaskInfo
{
    TaskInfo():
        kanbanObj(kanbanItem()),
        parent(0),
        addedTime(QTime::currentTime()),
        countScrap(0),
        countOpertators(2),
        done(false),
        scrapNote(QString()),
        mapped(false),
        taskWorkContent(0),
        lostTime(0),                      //hour
        lostTimeNote(QString()),          //hour
        curHour(-1)                       //hour 0-23. -1-task

    {}

    TaskInfo(const kanbanItem kanban, TaskInfo* parent = 0):
        kanbanObj(kanban),
        parent(parent),
        addedTime(QTime::currentTime()),
        countScrap(0),
        countOpertators(2),
        done(false),
        scrapNote(QString()),
        mapped(false),
        taskWorkContent(0),
        lostTime(0),                      //hour
        lostTimeNote(QString()),          //hour
        curHour(-1)                       //hour 0-23. -1-task

    {}
//    TaskInfo(const TaskInfo& taskInfo):
//        kanbanObj(taskInfo.kanbanObj),
//        parent(taskInfo.parent),
//        addedTime(taskInfo.addedTime),
//        countScrap(taskInfo.countScrap),
//        countOpertators(taskInfo.countOpertators),
//        done(taskInfo.done),
//        scrapNote(taskInfo.scrapNote),
//        mapped(taskInfo.mapped),
//        taskWorkContent(taskInfo.taskWorkContent),
//        lostTime(taskInfo.lostTime),                      //hour
//        lostTimeNote(taskInfo.lostTimeNote),          //hour
//        curHour(taskInfo.curHour)                       //hour 0-23. -1-task

//    {}

    bool operator ==(const TaskInfo& another) const
    {
        bool r = this->kanbanObj.kanban == another.kanbanObj.kanban;
        r = r && (this->addedTime == another.addedTime);
        Q_ASSERT(!r || this->kanbanObj.kanban == another.kanbanObj.kanban);
        Q_ASSERT(!r || this->taskWorkContent == another.taskWorkContent);
        Q_ASSERT(!r || this->countScrap == another.countScrap);
        Q_ASSERT(!r || this->done == another.done);
        Q_ASSERT(!r || this->scrapNote == another.scrapNote);
        Q_ASSERT(!r || this->lostTime == another.lostTime);
        Q_ASSERT(!r || this->parent == another.parent);
        Q_ASSERT(!r || this->lostTimeNote == another.lostTimeNote);
        Q_ASSERT(!r || this->curHour == another.curHour);
        Q_ASSERT(!r || this->mapped == another.mapped);
        Q_ASSERT(!r || this->children == another.children);
        return r;
    }

    QVector<TaskInfo> children;
    TaskInfo* parent;
    QTime addedTime;
    bool mapped;
    kanbanItem kanbanObj;
    int taskWorkContent;
    int countScrap;
    int countOpertators;
    bool done;
    QString scrapNote;
    int lostTime;               //hour
    QString lostTimeNote;       //hour
    int curHour;                //hour 0-23. -1-task

};

Planner::~Planner()
{}

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
//                        qDebug()<<kanbanObj.kanban<<kanbanObj.partWorkContent
//                                <<kanbanObj.countParts<<kanbanObj.reference<<kanbanObj.sebango;
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

void Planner::parseBuffer(const QByteArray &kanban)
{
    //TODO service commands
    if(kanban=="34703470"){
        qDebug()<<"refresh";
        readExcelData();
        return;
    }
    if(kanban.contains("scrap")) {
        //scrap +1
    }
        else addKanban(kanban);

//    switch(kanban){
//    case Planner::SC_SCRAP:
//    break;
//    default:
//        addKanban(kanban);
//    }

}

void Planner::addKanban(const QByteArray &kanban)
{
    qDebug()<<"kanban"<<kanban;
    if(kanban=="1")
        addKanban("1598001128");
    if(kanbanMap.contains(kanban)){
        TaskInfo task = TaskInfo(kanbanMap.value(kanban));
        task.taskWorkContent=task.kanbanObj.countParts*task.kanbanObj.partWorkContent;
        _notAttachedTasks.append(task);
        this->planBoardUpdate();
        return;
    }
        else qDebug()<<"Unknown command";
}

void Planner::planBoardUpdate()
{
    QTime ct=QTime::currentTime();
    //how many seconds are left until the end of an hour
    int hourValue=3600-ct.minute()*60+ct.second();
//    qDebug()<<"hourValue"<<hourValue<<"ct.minute()*60"<<ct.minute()*60<<"ct.second()"<<ct.second();

    int hourNumber=getCurrentHourNum();
    int startHour=getStartHourNum();
    int hoursCount=(QTime::currentTime().hour()<6)?6:9;
    //Shift change - clear tasks
    if(_tasks.count()!=hoursCount){
        _tasks.clear();
        for(int i=0;i<hoursCount;++i){
            TaskInfo newTask;
            newTask.curHour=startHour+i;
//            qDebug()<<"newTask.curHour"<<newTask.curHour<<"startHour"<<startHour;
            _tasks.append(newTask);
        }
    } else if(lastHour!=hourNumber)
        for(auto task=_tasks.begin();task!=_tasks.end();++task)
            for(auto child=task->children.begin();task!=task->children.end();++child)
                if(!child->done)
                    _notAttachedTasks.append(*child);
    lastHour=hourNumber;
    int hour=hourNumber;
//    qDebug()<<"hour"<<hour<<"hourValue1"<<hourValue;
    while(hour<_tasks.count() && _notAttachedTasks.count()>0){
        for(int i=0;i<_tasks.at(hour).children.count();++i)
            if(!_tasks.at(hour).children.at(i).done)
                if(_tasks.at(hour).taskWorkContent<hourValue)
                    hourValue-=_tasks.at(hour).taskWorkContent;
//        qDebug()<<"hour"<<hour<<"hourValue2"<<hourValue;
        for(int i=0;i<_notAttachedTasks.count();++i){
            if(_notAttachedTasks.at(0).taskWorkContent<hourValue){
//                qDebug()<<"hour"<<hour<<"hourValue3"<<hourValue<<"_tasks.count()"<<_tasks.count();
                hourValue-=_notAttachedTasks.at(0).taskWorkContent;
//                qDebug()<<"hour"<<hour<<"hourValue4"<<hourValue<<"_tasks.at(0).taskWorkContent"<<_tasks.at(0).taskWorkContent;
                _tasks[hour].taskWorkContent+=_notAttachedTasks.at(0).taskWorkContent;
                _notAttachedTasks[0].parent=&_tasks[hour];
                _tasks[hour].children.append(_notAttachedTasks.takeAt(0));
            }
        }
//        qDebug()<<"hour"<<hour<<"hourValue4.5"<<hourValue
//                <<"_tasks.at("<<hour<<").taskWorkContent"
//                <<_tasks.at(hour).taskWorkContent;
        hour++;
        hourValue+=3600;
    }
//    qDebug()<<"hour"<<hour<<"hourValue5";
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomRight = createIndex(rowCount(),columnCount());
//    qDebug()<<"hour"<<hour<<"hourValue6";
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, bottomRight);
//    for(int i=0;i<_tasks.count();++i)
//        for(int j=0;j<_tasks.at(i).children.count();++j)
//            qDebug()<<"_tasks.at("<<i<<").children.at("<<j<<").kanbanObj.kanban"
//                    <<_tasks.at(i).children.at(j).kanbanObj.kanban;
//    for(int i=0;i<_notAttachedTasks.count();++i)
//        qDebug()<<"_notAttachedTasks.at("<<i<<")"<<_notAttachedTasks.at(0).kanbanObj.kanban;


}

int Planner::getCurrentHourNum() const
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

int Planner::getStartHourNum() const
{
    QTime ct=QTime::currentTime();
    int startHour;
    if(ct.hour()<6)
        startHour=0;
    else if (ct.hour()<15)
        startHour=6;
    else
        startHour=15;
    return startHour;
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

int Planner::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return headers.count();
}

int Planner::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return _tasks.size();
    }
    const TaskInfo* parentInfo = static_cast<const TaskInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);

    return parentInfo->children.size();
}

QVariant Planner::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    const TaskInfo* taskInfo = static_cast<TaskInfo*>(index.internalPointer());

    switch(role){
    case Qt::DisplayRole: {
        int startHour=getStartHourNum();
        switch(col){
        case Columns::COL_PERIOD:
            return taskInfo->curHour>0 ? QString("%1.00-\n%2.00").arg(startHour+row).arg((startHour+row+1)%24) : QString();
        case Columns::COL_PLAN:{
            int plan=0;
            if(taskInfo->curHour>0)
                for(int i=0;i<taskInfo->children.count();++i)
                    plan+=taskInfo->children.at(i).kanbanObj.countParts;
            else
                plan=taskInfo->kanbanObj.countParts;
            return plan;
        }
        case Columns::COL_ACTUAL:{
            int act=0;
            if(taskInfo->curHour>0)
                for(int i=0;i<taskInfo->children.count();++i)
                    if(taskInfo->children.at(i).done)
                        act+=taskInfo->children.at(i).kanbanObj.countParts;
            else
                if(taskInfo->done)
                    act=taskInfo->kanbanObj.countParts;
            return act;
        }
        case Columns::COL_REFERENCE:{
            QString ref;
            if(taskInfo->curHour>0) {
                QMap<QByteArray,int> references;
                for(int i=0;i<taskInfo->children.count();++i)
                    if(references.contains(taskInfo->children.at(i).kanbanObj.reference))
                        references.insert(taskInfo->children.at(i).kanbanObj.reference,
                                           references.value(taskInfo->children.at(i).kanbanObj.reference)+
                                            taskInfo->children.at(i).kanbanObj.countParts);
                    else
                        references.insert(taskInfo->children.at(i).kanbanObj.reference,
                                           taskInfo->children.at(i).kanbanObj.countParts);
                for(int i=0;i<references.count();++i)
                    if(references.value(references.keys().at(i))>1)
                        ref.append(references.keys().at(i)).append(" x ")
                                .append(QString::number(references.value(references.keys().at(i)))).append("\n");
                    else
                        ref.append(references.keys().at(i)).append("\n");
            }
            else
                ref=taskInfo->kanbanObj.reference;
            return ref;
        }
        case Columns::COL_LOSTTIME:{
            if(taskInfo->parent!=0)
                return "";
            else
                return taskInfo->lostTime;
        }
        case Columns::COL_NOTES:{
            return taskInfo->lostTimeNote;
//            int note=0;
//            if(!taskInfo->lostTimeNote.isEmpty())
//                qDebug()<<"taskInfo->lostTimeNote"<<taskInfo->lostTimeNote;
//            if(!taskInfo->lostTimeNote.isEmpty())
//                note=lostTimeNoteList.indexOf(taskInfo->lostTimeNote);
//            qDebug()<<"note"<<note;
//            return note;
//            ActionsNoteList
        }
        case Columns::COL_SCRAP:{
            int scrap=0;
            if(taskInfo->parent==0)
                for(int i=0;i<taskInfo->children.count();++i)
                    scrap+=taskInfo->children.at(i).countScrap;
            else
                scrap=taskInfo->countScrap;
            return scrap;
        }
        case Columns::COL_OPERATORS:{
            QVariant countOpertators=0;
            if(taskInfo->parent==0)
                countOpertators=taskInfo->countOpertators;
            else
                countOpertators="";
            return countOpertators;
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
//        case Columns::COL_PERIOD:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_PLAN:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_ACTUAL:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_REFERENCE:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_LOSTTIME:
//            return data(this->index(row,col),Qt::DisplayRole);
        case Columns::COL_NOTES:{
            int note=0;
            if(!taskInfo->lostTimeNote.isEmpty())
                note=lostTimeNoteList.indexOf(taskInfo->lostTimeNote);
            return note;
        }
//        case Columns::COL_SCRAP:
//            return data(this->index(row,col),Qt::DisplayRole);
//            break;
        default:
            //return data(this->index(row,col),Qt::DisplayRole);
            return data(index,Qt::DisplayRole);
        }
    }
    }
    return QVariant();
}

bool Planner::setData(const QModelIndex & index, const QVariant & value, int role)
{
    TaskInfo* taskInfo = static_cast<TaskInfo*>(index.internalPointer());
    if (role == Qt::EditRole)
    {
        switch(index.column()){
        case Columns::COL_PERIOD:
            break;
        case Columns::COL_PLAN:
            break;
        case Columns::COL_ACTUAL:
            //TBD
            //taskInfo->children.at(0)->done=true;
            //value.toInt();
            break;
        case Columns::COL_REFERENCE:
            break;
        case Columns::COL_LOSTTIME:
            taskInfo->lostTime=value.toInt();
            break;
        case Columns::COL_NOTES:
            //qDebug()<<"COL_NOTES set index"<<value.toInt();
            taskInfo->lostTimeNote=lostTimeNoteList.at(value.toInt());
            break;
        case Columns::COL_SCRAP:
//            if(!taskInfo->children.isEmpty())
//                taskInfo->children[0].countScrap=value.toInt();
              taskInfo->countScrap=value.toInt();
            //huck
        case Columns::COL_OPERATORS:
           taskInfo->countOpertators=value.toInt();
            break;
        }
        emit editCompleted( value.toString() );
    }
    return true;
}

Qt::ItemFlags Planner::flags(const QModelIndex & index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    TaskInfo* taskInfo = static_cast<TaskInfo*>(index.internalPointer());
    int hourNumber = QTime::currentTime().hour();
    //if(index.row()<=hourNumber){
        switch(index.column()){
        //        case Columns::COL_ACTUAL:
        //            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        case Columns::COL_LOSTTIME:{
//            qDebug()<<"COL_LOSTTIME index.row()="<<index.row()<<"hourNumber="<<hourNumber
//                    <<"curHour="<<taskInfo->curHour<<"kanban="<<taskInfo->kanbanObj.kanban
//                    <<"children.count()="<<taskInfo->children.count()<<"parent="<<taskInfo->parent;
            if(taskInfo->curHour<=hourNumber && !taskInfo->parent){
//                qDebug()<<"COL_LOSTTIME index.row()="<<index.row()<<"hourNumber="<<hourNumber
//                        <<"curHour="<<taskInfo->curHour<<"kanban="<<taskInfo->kanbanObj.kanban
//                        <<"children.count()="<<taskInfo->children.count()<<"parent="<<taskInfo->parent;
                return flags | Qt::ItemIsEditable;
            } else
                return flags & ~Qt::ItemIsEditable;
        }
        case Columns::COL_SCRAP:{
//            qDebug()<<"COL_SCRAP index.row()="<<"hourNumber="<<hourNumber
//                    <<"curHour="<<taskInfo->curHour<<"kanban="<<taskInfo->kanbanObj.kanban
//                    <<"children.count()="<<taskInfo->children.count()<<"parent="<<taskInfo->parent;
            if(taskInfo->parent){
                TaskInfo* hourInfo = static_cast<TaskInfo*>(taskInfo->parent);
                if(hourInfo->curHour<=hourNumber){
//                    qDebug()<<"COL_SCRAP index.row()="<<"hourNumber="<<hourNumber
//                            <<"curHour="<<hourInfo->curHour<<"kanban="<<taskInfo->kanbanObj.kanban
//                            <<"children.count()="<<hourInfo->children.count()<<"parent="<<taskInfo->parent;
                return flags | Qt::ItemIsEditable;
                }
            }
            return flags & ~Qt::ItemIsEditable;
        }
        case Columns::COL_NOTES:{
            //            if(taskInfo->lostTimeNotes.isEmpty()||
            //                    taskInfo->lostTimeNotes=="0")
            if(taskInfo->curHour<=hourNumber)
                return flags | Qt::ItemIsEditable;
            else
                return flags & ~Qt::ItemIsEditable;
        }
        case Columns::COL_OPERATORS:{
            if(!taskInfo->parent)
                return flags | Qt::ItemIsEditable;
        }
        default:
            return flags & ~Qt::ItemIsEditable;
        }
    //}
    return flags & ~Qt::ItemIsEditable;
}



QModelIndex Planner::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if (!parent.isValid()) {
        Q_ASSERT(_tasks.size() > row);
        return createIndex(row, column, const_cast<TaskInfo*>(&_tasks[row]));
    }

    TaskInfo* parentInfo = static_cast<TaskInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);
    if(!parentInfo->mapped)
        parentInfo->mapped=true;
    Q_ASSERT(parentInfo->mapped);
    Q_ASSERT(parentInfo->children.size() > row);
    return createIndex(row, column, &parentInfo->children[row]);
}

QModelIndex Planner::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    TaskInfo* childInfo = static_cast<TaskInfo*>(child.internalPointer());
    if (childInfo == 0) {
        return QModelIndex();
    }
    Q_ASSERT(childInfo != 0);
    TaskInfo* parentInfo = childInfo->parent;
    if (parentInfo != 0) {
        return createIndex(findRow(parentInfo), COL_PERIOD, parentInfo);
    }
    else {
        return QModelIndex();
    }
}

int Planner::findRow(const TaskInfo *TaskInfo) const
{
    Q_ASSERT(TaskInfo != 0);
    const TaskInfoList& parentInfoChildren = TaskInfo->parent != 0 ? TaskInfo->parent->children: _tasks;
    TaskInfoList::const_iterator position = qFind(parentInfoChildren, *TaskInfo);
    Q_ASSERT(position != parentInfoChildren.end());
    return std::distance(parentInfoChildren.begin(), position);
}

bool Planner::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        const TaskInfo* parentInfo = static_cast<const TaskInfo*>(parent.internalPointer());
        Q_ASSERT(parentInfo != 0);
        if (!parentInfo->mapped) {
            return !parentInfo->children.isEmpty();
        }
    }
    return QAbstractItemModel::hasChildren(parent);
}

bool Planner::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }

    const TaskInfo* parentInfo = static_cast<const TaskInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);
    return !parentInfo->mapped;
}

void Planner::fetchMore(const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid());
    TaskInfo* parentInfo = static_cast<TaskInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != 0);
    Q_ASSERT(!parentInfo->mapped);
    if (parentInfo->children.count()>0)
        parentInfo->mapped=true;

}

bool Planner::saveExcelReport(const QString &fileName)
{
    qDebug()<<"saveExcelReport"<<"fileName";
    Document *xlsx= new Document(fileName);
    for(int j=0;j<headers.count();j++)
        xlsx->write(1,j+1,headers.value(headers.keys().at(j)));
    for(int j=0;j<headers.count();j++)
        for(int i=0;i<_tasks.count();i++)
            if(j==Columns::COL_NOTES)
                xlsx->write(i+2,j+1,lostTimeNoteList.at(data(createIndex(i,j)).toInt()));
            else
                xlsx->write(i+2,j+1,data(createIndex(i,j)));
    return xlsx->saveAs(fileName);
}

int Planner::getProgress()
{
    int workContent=0;
    int doneContent=0;
    for(int i=0;i<_tasks.count();i++)
        for(int j=0;j<_tasks[i].children.count();j++){
            workContent+=_tasks[i].children[j].taskWorkContent;
            if(_tasks[i].children[j].done)
                doneContent+=_tasks[i].children[j].taskWorkContent;
            //            qDebug()<<doneContent<<workContent;
        }
    //    qDebug()<<doneContent<<workContent;
    if(workContent==0)
        return 0;
    return doneContent*100/workContent;
}

