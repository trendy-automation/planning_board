#ifndef TASKINFO_H
#define TASKINFO_H
#include <QVector>
#include <QTime>
#include <QDataStream>
#include <QTextStream>
#include <QMap>
#include <QStringList>
#include <QDebug>



struct kanbanItem
{
    int countParts;
    int partWorkContent;
    QString reference;
    QString sebango;
    QString kanban;
    kanbanItem(){
        countParts=0;
        partWorkContent=0;
        reference="";
        sebango="";
        kanban="";
    }
};

Q_DECLARE_METATYPE(kanbanItem);

class TaskInfo
{
public:
    TaskInfo(const TaskInfo &other):
        parent(other.parent),
        kanbanObj(other.kanbanObj),
        addedTime(other.addedTime),
        //startedTime(other.startedTime),
        //completionTime(other.completionTime),
        countScrap(other.countScrap),
        countOpertators(other.countOpertators),
        running(other.running),
        mapped(other.mapped),
        done(other.done),
        canceled(other.canceled),
        scrapNote(other.scrapNote),
        taskWorkContent(other.taskWorkContent),
        lostTime(other.lostTime),                      //hour
        taskNote(other.taskNote),          //hour
        children(other.children),
        curHour(other.curHour)                       //hour 0-23. -1-task
    {}

    TaskInfo():
        kanbanObj(kanbanItem()),
        parent(nullptr),
        addedTime(QTime::currentTime()),
        //startedTime(QTime()),
        //completionTime(QTime()),
        countScrap(0),
        //TODO move countOpertators to excel
        countOpertators(3),
        running(false),
        done(false),
        canceled(false),
        scrapNote(QString()),
        mapped(false),
        taskWorkContent(0),
        lostTime(0),                      //hour
        taskNote(QString()),          //hour
        curHour(-1),                       //hour 0-23. -1-task
        children(QVector<TaskInfo>())
    {}

    TaskInfo(const kanbanItem kanban, TaskInfo* parent = 0):
        kanbanObj(kanban),
        parent(parent),
        addedTime(QTime::currentTime()),
        //startedTime(QTime()),
        //completionTime(QTime()),
        countScrap(0),
        countOpertators(1),
        running(false),
        done(false),
        canceled(false),
        scrapNote(QString()),
        mapped(false),
        taskWorkContent(0),
        lostTime(0),                      //hour
        taskNote(QString()),          //hour
        curHour(-1),                       //hour 0-23. -1-task
        children(QVector<TaskInfo>())
    {}


    //TODO:overload children.append

//    TaskInfo(const TaskInfo& taskInfo):
//        kanbanObj(taskInfo.kanbanObj),
//        parent(taskInfo.parent),
//        addedTime(taskInfo.addedTime),
//        countScrap(taskInfo.countScrap),
//        countOpertators(taskInfo.countOpertators),
//        running(taskInfo.running),
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
//        Q_ASSERT(!r || this->startedTime == another.startedTime);
//        Q_ASSERT(!r || this->completionTime == another.completionTime);
        Q_ASSERT(!r || this->kanbanObj.kanban == another.kanbanObj.kanban);
        Q_ASSERT(!r || this->taskWorkContent == another.taskWorkContent);
        Q_ASSERT(!r || this->countScrap == another.countScrap);
        Q_ASSERT(!r || this->running == another.running);
        Q_ASSERT(!r || this->done == another.done);
        Q_ASSERT(!r || this->canceled == another.canceled);
        Q_ASSERT(!r || this->scrapNote == another.scrapNote);
        Q_ASSERT(!r || this->lostTime == another.lostTime);
        Q_ASSERT(!r || this->parent == another.parent);
        Q_ASSERT(!r || this->taskNote == another.taskNote);
        Q_ASSERT(!r || this->curHour == another.curHour);
        Q_ASSERT(!r || this->mapped == another.mapped);
        Q_ASSERT(!r || this->children == another.children);
        return r;
    }

    bool operator <(const TaskInfo& another) const
    {
//        bool r = (!this->running and !another.running and (this->addedTime < another.addedTime)) or
//                  (this->running and !another.running);
        bool r = this->addedTime < another.addedTime;
        return r;
    }

    bool operator >(const TaskInfo& another) const
    {
        return this->addedTime > another.addedTime;
    }

    //TaskInfo takeTask(int i) const
    //{
    //    return children.at(i);
    //}

    QVector<TaskInfo> children;
    TaskInfo* parent;
    QTime addedTime;
    //QTime startedTime;
    //QTime completionTime;
    bool mapped;
    kanbanItem kanbanObj;
    int taskWorkContent;
    int countScrap;
    int countOpertators;
    bool running;
    bool done;
    bool canceled;
    QString scrapNote;
    int lostTime;               //hour
    QString taskNote;       //hour
    int curHour;                //hour 0-23. -1-task


    friend QDataStream &operator<<(QDataStream &out, const TaskInfo& tinf) {
        out<<tinf.done;
        out<<tinf.mapped;
        out<<tinf.parent;
        out<<tinf.curHour;
        out<<tinf.running;
        out<<tinf.canceled;
        //out<<tinf.children;
        out<<tinf.lostTime;
        out<<tinf.taskNote;
        out<<tinf.addedTime;
//        out<<tinf.startedTime;
//        out<<tinf.completionTime;
        out<<tinf.kanbanObj.kanban;
        out<<tinf.kanbanObj.sebango;
        out<<tinf.kanbanObj.reference;
        out<<tinf.kanbanObj.countParts;
        out<<tinf.kanbanObj.partWorkContent;
        out<<tinf.scrapNote;
        out<<tinf.countScrap;
        out<<tinf.countOpertators;
        out<<tinf.taskWorkContent;
        out<<tinf.children.count();
        foreach(auto child, tinf.children)
            out<<child;
       return out;
    }
    friend QDataStream &operator>>(QDataStream &in, TaskInfo &tinf) {
        in>>tinf.done;
        in>>tinf.mapped;
        tinf.parent=nullptr;
        in>>tinf.curHour;
        in>>tinf.running;
        in>>tinf.canceled;
        //in>>tinf.children;
        in>>tinf.lostTime;
        in>>tinf.taskNote;
        in>>tinf.addedTime;
//        in>>tinf.startedTime;
//        in>>tinf.completionTime;
        in>>tinf.kanbanObj.kanban;
        in>>tinf.kanbanObj.sebango;
        in>>tinf.kanbanObj.reference;
        in>>tinf.kanbanObj.countParts;
        in>>tinf.kanbanObj.partWorkContent;
        in>>tinf.scrapNote;
        in>>tinf.countScrap;
        in>>tinf.countOpertators;
        in>>tinf.taskWorkContent;
        int childrenCount;
        in>>childrenCount;
        for(int i=0;i<childrenCount;++i){
            if(!in.atEnd()) {
                TaskInfo child;
                in >> child;
                child.parent=&tinf;
                tinf.children.append(child);
            }
        }
       return in;
    }
    friend QStringList &operator<<(QStringList &out, const TaskInfo& tinf) {
        //QMap<QString, QString> taskMap;
        //taskMap.insert("done",tinf.done?"true":"false");
        //taskMap.insert("mapped",tinf.mapped?"true":"false");
        //out<<taskMap;
        //QVariantList out;
        //QStringList out;
        out<<(tinf.done?"true":"false");
        out<<(tinf.mapped?"true":"false");
        //out<<"0";//tinf.parent;
        out<<QString::number(tinf.curHour);
        out<<(tinf.running?"true":"false");
        out<<(tinf.canceled?"true":"false");
        //out<<tinf.children;
        out<<QString::number(tinf.lostTime);
        out<<tinf.taskNote;
        out<<tinf.addedTime.toString();
//        out<<tinf.startedTime;
//        out<<tinf.completionTime;
        out<<tinf.kanbanObj.kanban;
        out<<tinf.kanbanObj.sebango;
        out<<tinf.kanbanObj.reference;
        out<<QString::number(tinf.kanbanObj.countParts);
        out<<QString::number(tinf.kanbanObj.partWorkContent);
        out<<tinf.scrapNote;
        out<<QString::number(tinf.countScrap);
        out<<QString::number(tinf.countOpertators);
        out<<QString::number(tinf.taskWorkContent);
        out<<QString::number(tinf.children.count());
        foreach(auto child, tinf.children){
            QStringList childList;
            childList<<child;
            //qDebug()<<"childList<<child";
            out<<childList.join(CHILDREN_SEPARATOR);
        }
       return out;
    }

    friend QStringList &operator>>(QStringList &in, TaskInfo &tinf) {
        QStringList tmp=in;
        if(tmp.count()<18)
            return in;
        tinf.done=(tmp.takeAt(0)=="false"?false:true);
        tinf.mapped=(tmp.takeAt(0)=="false"?false:true);
        tinf.parent=nullptr;
        tinf.curHour=tmp.takeAt(0).toInt();
        tinf.running=(tmp.takeAt(0)=="false"?false:true);
        tinf.canceled=(tmp.takeAt(0)=="false"?false:true);
        tinf.lostTime=tmp.takeAt(0).toInt();
        tinf.taskNote=tmp.takeAt(0);
        tinf.addedTime=QTime::fromString(tmp.takeAt(0));
//        tinf.startedTime;
//        tinf.completionTime;
        tinf.kanbanObj.kanban=tmp.takeAt(0);
        tinf.kanbanObj.sebango=tmp.takeAt(0);
        tinf.kanbanObj.reference=tmp.takeAt(0);
        tinf.kanbanObj.countParts=tmp.takeAt(0).toInt();
        tinf.kanbanObj.partWorkContent=tmp.takeAt(0).toInt();
        tinf.scrapNote=tmp.takeAt(0);
        tinf.countScrap=tmp.takeAt(0).toInt();
        tinf.countOpertators=tmp.takeAt(0).toInt();
        tinf.taskWorkContent=tmp.takeAt(0).toInt();
        int childrenCount;
        childrenCount=tmp.takeAt(0).toInt();
        if(tmp.count()<childrenCount)
            return in;
        for(int i=0;i<childrenCount;++i){
                TaskInfo child;
                QStringList childList=tmp.takeAt(0).split(CHILDREN_SEPARATOR);
                childList >> child;
                child.parent=&tinf;
                tinf.children.append(child);
        }
       return in;
    }
};

Q_DECLARE_METATYPE(TaskInfo);

typedef QVector<TaskInfo> TaskInfoList;

#endif // TASKINFO_H
