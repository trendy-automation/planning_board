#ifndef TASKINFO_H
#define TASKINFO_H
#include <QVector>
#include <QTime>
#include <QDataStream>



struct kanbanItem
{
    int countParts;
    int partWorkContent;
    QByteArray reference;
    QByteArray sebango;
    QByteArray kanban;
    kanbanItem(){
        countParts=0;
        partWorkContent=0;
        reference="";
        sebango="";
        kanban="";
    }
};

class TaskInfo
{
public:
    TaskInfo(const TaskInfo &other):
        parent(other.parent),
        kanbanObj(other.kanbanObj),
        addedTime(other.addedTime),
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

    //TaskInfo takeTask(int i) const
    //{
    //    return children.at(i);
    //}

    QVector<TaskInfo> children;
    TaskInfo* parent;
    QTime addedTime;
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


    friend QDataStream &operator<<(QDataStream &out, const TaskInfo& rhs) {
       out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
       return out;
    }
    friend QDataStream &operator>>(QDataStream &in, TaskInfo &rhs) {
       in.readRawData(reinterpret_cast<char*>(&rhs), sizeof(rhs));
       return in;
    }
};

typedef QVector<TaskInfo> TaskInfoList;


//QDataStream &operator%(QDataStream &out, const TaskInfoList& rhs) {
//   out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
//   return out;
//}
//QDataStream &operator$(QDataStream &in, TaskInfoList &rhs) {
//   in.readRawData(reinterpret_cast<char*>(&rhs), sizeof(rhs));
//   return in;
//}



#endif // TASKINFO_H
