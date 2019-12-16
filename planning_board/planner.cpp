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
#include <QSpinBox>
#include <QApplication>
#include <QRect>
#include <QSize>
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
//#include <QTextStream>


//Q_DECLARE_METATYPE(TaskInfo)
Q_DECLARE_METATYPE(QVector<TaskInfo>)




Planner::Planner(QObject *parent) : QAbstractItemModel(parent)
    //,_metaProvider(new QFileIconProvider())

{
    qRegisterMetaType<TaskInfo>("TaskInfo>");
    qRegisterMetaType<QVector<TaskInfo>>("QVector<TaskInfo>");
    headers.insert(COL_PERIOD,"Период");
    headers.insert(COL_PLAN,"План");
    headers.insert(COL_ACTUAL,"Факт");
    headers.insert(COL_SEBANGO,"Себанго");
    headers.insert(COL_LOSTTIME,"Потерянное\nвремя");
//    headers.insert(COL_SCRAP,"Брак");
    headers.insert(COL_NOTES,"Замечания");
//    headers.insert(COL_OPERATORS,"Количество\nоператоров");
    headers.insert(COL_STATUS,"Статус");
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
        qDebug()<<"hourNumber"<<hourNumber;
            //_tasks.clear();
        this->planBoardUpdate();
    });
    QTime ct = QTime::currentTime();
    hourTimer->start(60000);
    //hourTimer->start(ct.msecsTo(QTime(ct.hour(),59,59))+2000);
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
    setProperty("statusList",statusList);
    setProperty("taskNoteList",taskNoteList);
    setProperty("scrapNoteList",scrapNoteList);

    //saveExcelReport("test_report");

    loadTasks();
    planBoardUpdate();
    //lastHour=-1;

//    QTimer * timerAddKanban = new  QTimer;
//    timerAddKanban->start(60000);
//    QObject::connect(timerAddKanban, &QTimer::timeout, this, [=](){
////    timerAddKanban->singleShot(120000, [=](){
////        addKanban("W76QD584YA");
//        kanbanProdused("W76QD584YA");
//    });
}


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
    taskNoteList.append("");
    while (ok && !note.isEmpty()){
        taskNoteList.append(note);
        i++;
        note = xlsx->read(i,1).toByteArray();
    }
    if(!kanbanMap.isEmpty()){
        avgWorkContent=0;
        for(auto kanban=kanbanMap.begin();kanban!=kanbanMap.end();++kanban)
            avgWorkContent+=kanban.value().partWorkContent*kanban.value().countParts;
        avgWorkContent=avgWorkContent/kanbanMap.count();
        qDebug()<<"avgWorkContent"<<avgWorkContent;
    }
    return ok;
}

void Planner::parseBuffer(const QByteArray &inputText)
{
    qDebug()<<inputText;
    QByteArray inputData=inputText;
    inputData.replace("\r","");
    inputData.replace("\n","");
    inputData.replace("\xA0","");
    inputData.replace("\x10","");
    inputData.replace("\u0001","");
    inputData.replace("\u0010","");
    inputData.replace("?","");
    //TODO service commands
    if (inputData=="XRENEW"){
        qDebug()<<"refresh_excel";
        readExcelData();
        return;
    }
    bool ok = false;
    if (inputData.startsWith("HOUR")){
        notesHour=inputData.right(2).toInt(&ok,10);
        //qDebug()<<"notesHour"<<notesHour;
        if(ok && _tasks.count()>notesHour)
            planBoardUpdate();
        return;
    }
    if (inputData.startsWith("NOTE")){
        int noteNum = inputData.right(2).toInt(&ok,10);
        qDebug()<<"noteNum"<<noteNum<<taskNoteList.at(noteNum);
        if(ok && taskNoteList.count()>noteNum && noteNum>=0)
            if (_tasks.count()>notesHour && notesHour>=0){
                _tasks[notesHour].taskNote=_tasks[notesHour].taskNote.append("\n")
                        .append(taskNoteList.at(noteNum));
            }
        notesHour=-1;
        planBoardUpdate();
        return;
    }
        addKanban(inputData);
        qDebug()<<"finish parseBuffer";
}
void Planner::startSMED()
{
    TaskInfo task = TaskInfo();
    kanbanItem kanbanObj;
    kanbanObj.kanban="SMED";
    kanbanObj.reference="Идёт SMED";
    kanbanObj.sebango="Идёт SMED";
    task.kanbanObj=kanbanObj;
    task.running=true;
    qDebug()<<"&_tasks[getCurrentHourNum()] 2";
    task.parent=&_tasks[getCurrentHourNum()];
    _tasks[getCurrentHourNum()].children.append(task);
    qDebug()<<"&_tasks[getCurrentHourNum()] 2 fin";
    this->planBoardUpdate();
}

void Planner::finishSMED()
{        
    for(auto task=_tasks.begin();task!=_tasks.end();++task)
        for(auto child=task->children.begin();child!=task->children.end();++child)
            if(child->kanbanObj.kanban=="SMED" && child->running){
                child->kanbanObj.reference="SMED";
                child->kanbanObj.sebango="SMED";
                child->running=false;
                child->done=true;
                //_tasks[getCurrentHourNum()].children.append(*child);
                child->kanbanObj.partWorkContent=child->addedTime.elapsed()/60000;
                this->planBoardUpdate();
                return;
            }

    //TODO: если смед в следующих часах.
    TaskInfo task = TaskInfo();
    kanbanItem kanbanObj;
    kanbanObj.kanban="SMED";
    kanbanObj.reference="SMED";
    kanbanObj.sebango="SMED";
    task.kanbanObj=kanbanObj;
    task.done=true;
    qDebug()<<"&_tasks[getCurrentHourNum()] 3";
    task.parent=&_tasks[getCurrentHourNum()];
    //TODO move smed workcontent to excel
    task.taskWorkContent=200;
    _tasks[getCurrentHourNum()].children.append(task);
    qDebug()<<"&_tasks[getCurrentHourNum()] 3 fin";
    this->planBoardUpdate();
}

void Planner::addKanban(const QByteArray &kanban)
{
    if(kanbanMap.contains(kanban)){
        TaskInfo task = TaskInfo(kanbanMap.value(kanban));
        task.taskWorkContent=task.kanbanObj.countParts*task.kanbanObj.partWorkContent;
        //task.done=true;
        _notAttachedTasks.append(task);
        this->planBoardUpdate();
        qDebug()<<"kanban"<<QString(kanban);
        return;
    }
        else {
            qDebug()<<"Unknown command"<<QString(kanban);
    }
}

void Planner::saveTasks(){
    qDebug()<<"save tasks to tasks.txt";
    QSettings settings("tasks.txt", QSettings::IniFormat);
    qDebug()<<"setIniCodec";
    settings.setIniCodec("UTF-8");
    foreach(auto key, settings.allKeys()){
        //qDebug()<<"settings.remove(key)";
        settings.remove(key);
    }
    settings.beginGroup("Tasks");
    settings.beginWriteArray("_tasks");
    for(int i=0;i<_tasks.count();++i){
        //qDebug()<<"str <<_tasks[i]";
        settings.setArrayIndex(i);
        QStringList str;
        //qDebug()<<"str <<_tasks[i]";
        str <<_tasks[i];
        //qDebug()<<"setValue";
        settings.setValue(QByteArray("Task")+QByteArray::number(i),str.join(TASK_SEPARATOR));
//        foreach(auto child, _tasks[i].children)
//            qDebug()<<i<<child.kanbanObj.sebango;
    }
    settings.endArray();
    settings.setValue("Date",QDate::currentDate().toString(DATA_FORMAT));
    settings.setValue("Shift",getShiftNum());
    settings.setValue("lastHour",lastHour);
    settings.endGroup();
    qDebug()<<"done";
}

void Planner::loadTasks(){
    qDebug()<<"load tasks from tasks.txt";
    QSettings settings("tasks.txt", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("Tasks");
    QDate settingsDate = QDate::fromString(settings.value("Date",QString()).toString(),DATA_FORMAT); //toString
    int settingsSift = settings.value("Shift",0).toInt();
    if (settingsSift!=getShiftNum() || settingsDate!=QDate::currentDate()){
        foreach(auto key, settings.allKeys())
            settings.remove(key);
        qDebug()<<"old data skiped";
        return;
    }
    lastHour = settings.value("lastHour",-1).toInt();
    int size = settings.beginReadArray("_tasks");
    _tasks.clear();
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        TaskInfo task;
        QString str;
        QStringList task_str;
        str = settings.value(QByteArray("Task")+QByteArray::number(i),QString()).toString();
        task_str=str.split(TASK_SEPARATOR);
        //qDebug()<<"task_str"<<task_str;
        task_str>>task;
        _tasks.append(task);
//        foreach(auto child, _tasks[i].children)
//            qDebug()<<i<<child.kanbanObj.sebango;
    }
    settings.endArray();
    settings.endGroup();
    planBoardUpdate();
    qDebug()<<"done";
}

void Planner::planBoardUpdate(/*bool forceUpdate*/)
{
    emit uiEnable(false);
    QTime ct=QTime::currentTime();
    //how many seconds are left until the end of an hour
    int hourNumber=getCurrentHourNum();
    //qDebug()<<"hourNumber"<<hourNumber;
    qDebug()<<"lastHour"<<lastHour<<"hourNumber"<<hourNumber<<"notesHour"<<notesHour;
    //Shift change - clear tasks
    //lastHour=-1;
    int runHour = -1;
    for(int i=0;i<_tasks.count();++i) {
        //qDebug()<<"_tasks.count()"<<_tasks.count()<<"i"<<i;
        if(_tasks.at(i).running) {
            //qDebug()<<"_tasks.at(i).running"<<i;
            if(runHour==-1)
                runHour = i;
            else {
                qDebug()<<"!!! Extra _tasks[i].running; i="<<i<< "lastHour="<<lastHour;
                _tasks[i].running=false;
            }
        }
    }
    if (lastHour!=runHour && lastHour!=-1){
        qDebug()<<"!!! WAF?! runHour="<<runHour<<" lastHour="<<lastHour;
    }

    if(lastHour==-1 || lastHour!=hourNumber || _tasks.isEmpty()){
        if(lastHour==-1 || ( lastHour!=hourNumber && hourNumber==0) ||_tasks.isEmpty()){
            //start next shift
            //_tasks.clear();
            startNextShift();
        } else {
            qDebug()<<1;
            if((_tasks.count()>lastHour) && !_tasks.isEmpty()){
                qDebug()<<11;
                if(!_tasks.at(lastHour).children.isEmpty()){
                    qDebug()<<111;
                    for(int j=_tasks.at(lastHour).children.count()-1;j>=0;--j){
                        qDebug()<<12;
                        if (_tasks.at(lastHour).children.at(j).running){
                            qDebug()<<13;
                            int timeElapsed = _tasks.at(lastHour).children.at(j).addedTime.elapsed()/1000;
                            int taskWorkContent=qMax(0,_tasks.at(lastHour).children.at(j).taskWorkContent-timeElapsed);
                            if (taskWorkContent==0){
                                qDebug()<<14;
                                _tasks[lastHour].children[j].lostTime=(timeElapsed-_tasks.at(lastHour).children.at(j).taskWorkContent)/60;
                            }
                            qDebug()<<2;
                            //TODO: timeElapsed and taskWorkContent function
                            TaskInfo newTask(_tasks.at(lastHour).children.at(j));
                            qDebug()<<21;
                            newTask.lostTime=0;
                            qDebug()<<22;
                            newTask.taskWorkContent=taskWorkContent;
                            qDebug()<<23;
                            newTask.parent=nullptr;
                            qDebug()<<24;
                            _notAttachedTasks.append(newTask);
                            qDebug()<<25;
                            _tasks[lastHour].children[j].taskWorkContent=timeElapsed;
                            qDebug()<<26;
                            _tasks[lastHour].children[j].running=false;
                            qDebug()<<27;
                            _tasks[lastHour].children[j].done=true;
                            qDebug()<<3;
                        }
                    }
                }
                qDebug()<<31;
//                hourHasChanged(hourNumber);
            }
            qDebug()<<32;
            //copy runned tasks to next hour
        }
        qDebug()<<33;
    }
    lastHour=hourNumber;
    int lostTime;
    int hourValue=(3600-ct.minute()*60+ct.second());//*_tasks.at(hourNumber).countOpertators;
    qDebug()<<34<<"_tasks.count()"<<_tasks.count();
    //move all kanban tasks to not attached
    for(int i=0;i<_tasks.count();++i)
        for(int j=0;j<_tasks.at(i).children.count();++j)
            if(!_tasks.at(i).children.at(j).running &&
                    !_tasks.at(i).children.at(j).done &&
                    !_tasks.at(i).children.at(j).canceled){
                    qDebug()<<35;
                    _tasks[i].children[j].parent=nullptr;
                    _tasks[i].taskWorkContent=qMax(0,_tasks[i].taskWorkContent-_tasks.at(i).children.at(j).taskWorkContent);
                _notAttachedTasks.append(_tasks[i].children.takeAt(j));
                qDebug()<<4;
            }
            else {
                if (_tasks.at(i).children.at(j).running){
                    int timeElapsed = _tasks.at(i).children.at(j).addedTime.elapsed()/1000;
                    //int taskWorkContent=qMax(0,_tasks.at(i).children.at(j).taskWorkContent-timeElapsed);
                    //int taskWorkContent =_tasks[i].children[j].taskWorkContent-timeElapsed;
                    //if (taskWorkContent==0){
                    if (timeElapsed> _tasks.at(i).children.at(j).taskWorkContent){
                        _tasks[i].children[j].lostTime=(timeElapsed-_tasks.at(i).children.at(j).taskWorkContent)/60;
                    }
                    qDebug()<<5;
                }
            }
        //qDebug()<<"task->children.count()"<<task->children.count();

    qDebug()<<6;
    qSort(_notAttachedTasks.begin(), _notAttachedTasks.end(),
          [](const TaskInfo &t1,const TaskInfo &t2){return t1<t2;});
    int hour=hourNumber;
    qDebug()<<"hour"<<hour<<"hourValue1"<<hourValue;
//    qDebug()<<"_tasks.count()"<<_tasks.count();
//    for(int i = 0 ;i<_notAttachedTasks.count();++i)
//        qDebug()<<i<<_notAttachedTasks.at(i).kanbanObj.sebango
//                <<_notAttachedTasks.at(i).addedTime.toString("h:mm:ss");

    for(int h=0;h<hour+1;++h){
        if (h==hourNumber)
            lostTime=3600-hourValue;
        else
            lostTime=3600;
        for(int i=0;i<_tasks.at(h).children.count();++i)
            if(_tasks.at(h).children.at(i).done && !_tasks.at(h).children.at(i).canceled){
                    lostTime-=_tasks.at(h).children.at(i).taskWorkContent;
                    lostTime+=_tasks.at(h).children.at(i).lostTime;
            }
        _tasks[h].lostTime=qMax(0,lostTime/60);
//        qDebug()<<"hour"<<hour<<"_tasks[hour].lostTime"<<_tasks[hour].lostTime;
        qDebug()<<7;
    }

    while(hour<_tasks.count() ){//&& _notAttachedTasks.count()>0){
        for(int i=0;i<_tasks.at(hour).children.count();++i)
            if(!_tasks.at(hour).children.at(i).done && !_tasks.at(hour).children.at(i).canceled)
                if(_tasks.at(hour).children.at(i).taskWorkContent<hourValue)
                    hourValue-=_tasks.at(hour).children.at(i).taskWorkContent;
            qDebug()<<8;
//        qDebug()<<"hour"<<hour<<"hourValue2"<<hourValue;
        while(!_notAttachedTasks.isEmpty()){
            if(hourValue>_notAttachedTasks.first().taskWorkContent){
//                qDebug()<<"hour"<<hour<<"hourValue3"<<hourValue<<"_tasks.count()"<<_tasks.count();
                hourValue-=_notAttachedTasks.first().taskWorkContent;
//                qDebug()<<"hour"<<hour<<"hourValue4"<<hourValue<<"_notAttachedTasks.first().taskWorkContent"
//                        <<_notAttachedTasks.first().taskWorkContent;
                _tasks[hour].taskWorkContent+=_notAttachedTasks.first().taskWorkContent;
                _notAttachedTasks[0].parent=&_tasks[hour];
                //qDebug()<<"_notAttachedTasks[0].kanbanObj.reference"<<
                //           _notAttachedTasks[0].kanbanObj.reference<<"hour"<<hour;
                _tasks[hour].children.append(_notAttachedTasks.takeFirst());
                qDebug()<<9;
            }
        }
//                <<"_tasks.at("<<hour<<").taskWorkContent"
//                <<_tasks.at(hour).taskWorkContent;
        hour++;
        hourValue+=3600;//*_tasks.at(hour).countOpertators;
        //qDebug()<<"hour"<<hour<<"hourValue5"<<hourValue
        //        <<"_tasks.at(hour).countOpertators"<<_tasks.at(hour).countOpertators;
    }
    //qDebug()<<100;
//    qDebug()<<"hour"<<hour<<"hourValue5";
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomRight = createIndex(rowCount(),columnCount());
    qDebug()<<"hour"<<hour<<"hourValue6";
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, bottomRight);
    emit uiEnable(true);
    qDebug()<<"lastHour"<<lastHour<<"hourNumber"<<hourNumber;
////    for(int i=0;i<_tasks.count();++i)
////        for(int j=0;j<_tasks.at(i).children.count();++j)
////            qDebug()<<"_tasks.at("<<i<<").children.at("<<j<<").kanbanObj.kanban"
////                    <<_tasks.at(i).children.at(j).kanbanObj.kanban;
//    for(int i=0;i<_notAttachedTasks.count();++i)
//        qDebug()<<"_notAttachedTasks.at("<<i<<")"<<_notAttachedTasks.at(0).kanbanObj.kanban;

    if (_tasks.count()>hourNumber)
        _tasks[hourNumber].running=true;
    else
        qDebug()<<"!!!!_tasks.count()<=hourNumber";
    saveTasks();
}

void Planner::startNextShift()
{
    qDebug()<<"startNextShift";
    int hoursCount=getShiftHoursCount();
    int startHour=getStartHourNum();
    //save excel report
    if(!_tasks.isEmpty()){
        int shiftNum=0;
        if (startHour==0)  shiftNum=2;
        if (startHour==6)  shiftNum=3;
        if (startHour==15) shiftNum=1;
        //saveExcelReport(QDate().currentDate().toString(
        //                    QString("Report_dd_MM_yy_shift_%1.xlsx").arg(shiftNum)));
    }
    _notAttachedTasks.clear();
    _notAttachedTasks.squeeze();
    for(int i=0;i<_tasks.count();++i){
        _tasks[i].children.clear();
        _tasks[i].children.squeeze();
    }
    _tasks.clear();
    _tasks.squeeze();

//    for(int i=0;i<_notAttachedTasks.count();++i){
//        qDebug()<<"_notAttachedTasks.removeOne(*natask);";
//        _notAttachedTasks.removeOne(*natask);
//    }
//    for(auto task=_tasks.begin();task!=_tasks.end();++task){
//        for(auto child=task->children.begin();child!=task->children.end();++child){
//            qDebug()<<"task->children.removeOne(*child);";
//            task->children.removeOne(*child);
//        }
//        qDebug()<<"_tasks.removeOne(*task);";
//        _tasks.removeOne(*task);
//    }
    for(int i=0;i<hoursCount;++i){
        qDebug()<<"newTask";
        TaskInfo newTask;
        newTask.curHour=startHour+i;
        _tasks.append(newTask);
    }

    //TODO move KnownTasks to excel
    switch (startHour) {
    case 0:
        addKnownTask("ТОП 5",1-1,5*60);
        addKnownTask("Первая годная",1-1,10*60);
        addKnownTask("Перерыв",2-1,15*60);
        addKnownTask("Обед",3-1,30*60);
        addKnownTask("5S",6-1,15*60);
        break;
    case 6:
        addKnownTask("ТОП 5",6-startHour,5*60);
        addKnownTask("Первая годная",6-startHour,10*60);
        addKnownTask("Перерыв",8-startHour,15*60);
        addKnownTask("Обед",10-startHour,30*60);
        addKnownTask("Перерыв",13-startHour,15*60);
        addKnownTask("5S",14-startHour,15*60);
        break;
    case 15:
        qDebug()<<"hourNumber 15";
        addKnownTask("ТОП 5",15-startHour,5*60);
        addKnownTask("Первая годная",15-startHour,10*60);
        addKnownTask("Перерыв",17-startHour,15*60);
        addKnownTask("Обед",18-startHour,30*60);
        addKnownTask("Перерыв",21-startHour,15*60);
        addKnownTask("5S",23-startHour,15*60);
        break;
    default:{
        qDebug()<<"hourNumber error";
        }
    }
    for(auto task=_tasks.begin();task!=_tasks.end();++task){
        int wc = 0;
        for(auto child=task->children.begin();child!=task->children.end();++child)
            wc+=child->taskWorkContent;
        task->taskWorkContent=wc;
        //task->plan=(3600-wc)/avgWorkContent
    }

}

void Planner::addKnownTask(const QByteArray &sebango,int hourNum,int workContent)
{
    TaskInfo task = TaskInfo();
    kanbanItem kanbanObj;
    kanbanObj.sebango=sebango;
    kanbanObj.countParts=0;
    task.kanbanObj=kanbanObj;
    task.taskWorkContent=workContent;
    task.done=true;
    task.parent=&_tasks[hourNum];
    if(_tasks.count()>hourNum && hourNum>=0)
        _tasks[hourNum].children.append(task);
    else
        qDebug()<<"wrong hourNum"<<hourNum<<"_tasks.count()"<<_tasks.count();
    //this->planBoardUpdate();
    qDebug()<<"addKnownTask finish";
}

int Planner::getCurrentHourNum() const
{
    QTime ct=QTime::currentTime();
    int hourNumber;
    if(ct.hour()<6)
        hourNumber=ct.hour();
    else {
        if (ct.hour()<15)
            hourNumber=ct.hour()-6;
        else
            hourNumber=ct.hour()-15;
        }
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

int Planner::getShiftNum() const
{
    QTime ct=QTime::currentTime();
    int shiftNum;
    if(ct.hour()<6)
        shiftNum=3;
    else if (ct.hour()<15)
        shiftNum=1;
    else
        shiftNum=2;
    return shiftNum;
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
//        switch(index.column()){
//        case Columns::COL_SEBANGO:
//            return Qt::AlignVCenter;
//        default:
//            return Qt::AlignCenter;
//        }
//    {
        return Qt::AlignCenter;
        break;
//    }
    case Qt::SizeHintRole:
        if (orientation == Qt::Horizontal) {
            //                        switch(section){
            //                        case Columns::COL_PERIOD:
            //                            return QSize(250,70);
            //                        case Columns::COL_PLAN:
            //                            return QSize(50,70);
            //                        case Columns::COL_ACTUAL:
            //                            return QSize(50,70);
            //                        case Columns::COL_SEBANGO:
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
    Q_ASSERT(parentInfo);

    return parentInfo->children.size();
}

QVariant Planner::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
//    qDebug()<<"row"<<row<<"col"<<"role"<<role;
    const TaskInfo* taskInfo = static_cast<TaskInfo*>(index.internalPointer());

    if (taskInfo==nullptr){
        qDebug()<<"taskInfo==nullptr";
        return QVariant();
    }

    switch(role){
    case Qt::DisplayRole: {
        int startHour=getStartHourNum();
        switch(col){
        case Columns::COL_PERIOD:
            return (taskInfo->curHour>=0) ? QString("%1.00-\n%2.00").arg(startHour+row).arg((startHour+row+1)%24) : QString();
        case Columns::COL_PLAN:{
            int plan=0;
            if(!taskInfo->parent){
                int planned_stops=0;
                for(int i=0;i<taskInfo->children.count();++i)
                    if(taskInfo->children.at(i).kanbanObj.countParts==0 &&
                            taskInfo->children.at(i).done)
                        planned_stops+=taskInfo->children.at(i).taskWorkContent;
                plan=(3600-planned_stops)/avgWorkContent;
            }
            else
                plan=taskInfo->kanbanObj.countParts;
            return plan;
            //TODO plan
        }
        case Columns::COL_ACTUAL:{
            int act=0;
            if(!taskInfo->parent){
                for(int i=0;i<taskInfo->children.count();++i)
                    if(taskInfo->children.at(i).done)
                        act+=taskInfo->children.at(i).kanbanObj.countParts;
            }
            else
                if(taskInfo->done)
                    act=taskInfo->kanbanObj.countParts;
            return act;
        }
        case Columns::COL_SEBANGO:{
            QString ref;
            if(!taskInfo->parent && !taskInfo->children.isEmpty()) {
                QMap<QString,int> sebangoMap;
                QVector<TaskInfo> children = taskInfo->children;
                qSort(children.begin(), children.end(),
                      [](const TaskInfo &t1,const TaskInfo &t2){return t1<t2;});
                //qDebug()<<1;
                for(int i=0;i<children.count();++i)
                    if(sebangoMap.contains(children.at(i).kanbanObj.sebango))
                        sebangoMap.insert(children.at(i).kanbanObj.sebango,
                                           sebangoMap.value(children.at(i).kanbanObj.sebango)+
                                            children.at(i).kanbanObj.countParts);
                    else
                        sebangoMap.insert(children.at(i).kanbanObj.sebango,
                                           children.at(i).kanbanObj.countParts);
                //qDebug()<<2;
                for(int i=0;i<sebangoMap.count();++i){
                    if(sebangoMap.value(sebangoMap.keys().at(i))>1)
                        ref.append(sebangoMap.keys().at(i)).append(" x ")
                                .append(QString::number(sebangoMap.value(sebangoMap.keys().at(i))));
                    else
                        ref.append(sebangoMap.keys().at(i));
                    if(i!=sebangoMap.count()-1)
                        ref.append("\n");
                }
            }
            else
                ref=taskInfo->kanbanObj.sebango;
            return ref;
        }
        case Columns::COL_LOSTTIME:{
//            if(taskInfo->parent)
//                return 0;
//            else
                return taskInfo->lostTime;
        }
        case Columns::COL_NOTES:{
            if(!taskInfo->children.isEmpty())
                for(int i=0;i<taskInfo->children.count();++i)
                    if(!taskInfo->children.at(i).taskNote.isEmpty())
                        return taskNoteList.last();
            return taskInfo->taskNote;
//            int note=0;
//            if(!taskInfo->lostTimeNote.isEmpty())
//                qDebug()<<"taskInfo->lostTimeNote"<<taskInfo->lostTimeNote;
//            if(!taskInfo->lostTimeNote.isEmpty())
//                note=lostTimeNoteList.indexOf(taskInfo->lostTimeNote);
//            qDebug()<<"note"<<note;
//            return note;
//            ActionsNoteList
        }
//        case Columns::COL_SCRAP:{
//            int scrap=0;
//            if(!taskInfo->parent)
//                for(int i=0;i<taskInfo->children.count();++i)
//                    scrap+=taskInfo->children.at(i).countScrap;
//            else
//                scrap=taskInfo->countScrap;
//            return scrap;
//        }
//        case Columns::COL_OPERATORS:{
//            QVariant countOpertators=0;
//            if(!taskInfo->parent)
//                countOpertators=taskInfo->countOpertators;
//            else
//                countOpertators="";
//            return countOpertators;
//        }
        case Columns::COL_STATUS:{
            int status=0;
            if(taskInfo->parent){
                if(taskInfo->done)
                    status=1;
                if(taskInfo->canceled)
                    status=2;
                if(taskInfo->running)
                    status=3;
            }
            return statusList.at(status);
            }
        default:
            return QVariant();
        }
        break;
    }
//    case Qt::DecorationRole
//        if (col==Columns::COL_PERIOD)
//            return iconProvider.icon(QFileIconProvider::Folder);
//        breack;
    case Qt::FontRole:
        return QFont(FONT_TYPE,FONT_VALUE);
        //break;
    case Qt::BackgroundRole:
        //QBrush redBackground(Qt::red);
        //return redBackground;
        switch (col) {
        case Columns::COL_PERIOD:{
            //if (row==notesHour) ячейки строк?
            //минус номер первой видной строки
            if(taskInfo->curHour>=0 && taskInfo->curHour-getStartHourNum()==notesHour)
                return QBrush(Qt::cyan);
            break;
        }
        case Columns::COL_ACTUAL:{
            if(taskInfo->curHour>=0 && taskInfo->curHour-getStartHourNum()<=getCurrentHourNum()){
//                int lostParts=0;
//                for(int i=0;i<taskInfo->children.count();++i)
//                    lostParts+=taskInfo->children.at(i).kanbanObj.countParts;
                if(taskInfo->lostTime>0){
                    if(taskInfo->lostTime>10)
                        return QBrush(Qt::red);
                    else
                        return QBrush(Qt::yellow);
                } else
                    return QBrush(Qt::green);
            }
            break;
        }
//        case Columns::COL_LOSTTIME:{
//            if(taskInfo->curHour>=0 && taskInfo->curHour-getStartHourNum()<=getCurrentHourNum()){
//                if(taskInfo->lostTime>0){
//                    if(taskInfo->lostTime>10)
//                        return QBrush(Qt::red);
//                    else
//                        return QBrush(Qt::yellow);
//                } else
//                    return QBrush(Qt::green);
//            }
//            break;
//        }
        //default:
        //    return QVariant();
        //break;
        }
        return QVariant();
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        //break;
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
//    case Qt::SizeHintRole:{
//        switch(col){
//        case Columns::COL_PERIOD:{
//            return qMax(data(this->index(row,COL_SEBANGO),Qt::SizeHintRole),58);
//        }
//        case Columns::COL_SEBANGO:{
//            return qMax(data(this->index(row,COL_PERIOD),Qt::SizeHintRole),58);
//        }
//        default:{
//        }
//        }
//    }
    case Qt::EditRole:{
        switch(col){
//        case Columns::COL_PERIOD:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_PLAN:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_ACTUAL:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_SEBANGO:
//            return data(this->index(row,col),Qt::DisplayRole);
//        case Columns::COL_LOSTTIME:
//            return data(this->index(row,col),Qt::DisplayRole);
        case Columns::COL_NOTES:{
            int note=0;
            if(!taskInfo->taskNote.isEmpty())
                note=taskNoteList.indexOf(taskInfo->taskNote);
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
//    case Qt::SizeHintRole:{
//        switch(col){
//        case Columns::COL_SEBANGO:{
//            //this->data(index, Qt::SizeHintRole).
//            QSize baseSize(/*getFixedWidth(col)*/ 200, 0);
//            baseSize.setHeight(10000);
//            //something very high, or the maximum height of your text block

//            QFontMetrics metrics(this->data(index, Qt::FontRole).value<QFont>());
//            QRect outRect = metrics.boundingRect(QRect(QPoint(0, 0), baseSize), Qt::AlignLeft,
//                                                 this->data(index, Qt::DisplayRole).toString());
//            baseSize.setHeight(outRect.height());
//            return baseSize;
//        }
//        }
//    }
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
        case Columns::COL_SEBANGO:
            break;
        case Columns::COL_LOSTTIME:
            taskInfo->lostTime=value.toInt();
            break;
        case Columns::COL_NOTES:
            //qDebug()<<"COL_NOTES set index"<<value.toInt();
            taskInfo->taskNote=taskNoteList.at(value.toInt());
            break;
//        case Columns::COL_SCRAP:
////            if(!taskInfo->children.isEmpty())
////                taskInfo->children[0].countScrap=value.toInt();
//              taskInfo->countScrap=value.toInt();
//            //huck
//        case Columns::COL_OPERATORS:{
//            taskInfo->countOpertators=value.toInt();
//            planBoardUpdate();
//            qDebug()<<"planBoardUpdate();";
//            break;
//        }
        case Columns::COL_STATUS:
            switch (value.toInt()) {
            case 0: //in progress
                taskInfo->done=false;
                taskInfo->canceled=false;
                break;
            case 1: //done
                taskDone(taskInfo);
                break;
            case 2: //canceled
                taskCancel(taskInfo);
                break;
            default:
                qDebug()<<"Unknown status";
                break;
            }
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
    int hourNumber = getCurrentHourNum();
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
//        case Columns::COL_SCRAP:{
////            qDebug()<<"COL_SCRAP index.row()="<<"hourNumber="<<hourNumber
////                    <<"curHour="<<taskInfo->curHour<<"kanban="<<taskInfo->kanbanObj.kanban
////                    <<"children.count()="<<taskInfo->children.count()<<"parent="<<taskInfo->parent;
//            if(taskInfo->parent){
//                TaskInfo* hourInfo = static_cast<TaskInfo*>(taskInfo->parent);
//                if(hourInfo->curHour<=hourNumber){
////                    qDebug()<<"COL_SCRAP index.row()="<<"hourNumber="<<hourNumber
////                            <<"curHour="<<hourInfo->curHour<<"kanban="<<taskInfo->kanbanObj.kanban
////                            <<"children.count()="<<hourInfo->children.count()<<"parent="<<taskInfo->parent;
//                return flags | Qt::ItemIsEditable;
//                }
//            }
//            return flags & ~Qt::ItemIsEditable;
//        }
        case Columns::COL_NOTES:{
            //            if(taskInfo->lostTimeNotes.isEmpty()||
            //                    taskInfo->lostTimeNotes=="0")
            if(taskInfo->parent)
               if(taskInfo->parent->curHour==hourNumber)
                   return flags | Qt::ItemIsEditable;
            return flags & ~Qt::ItemIsEditable;
        }
//        case Columns::COL_OPERATORS:{
//            if(!taskInfo->parent)
//                return flags | Qt::ItemIsEditable;
//        }
        case Columns::COL_STATUS:{
            if(taskInfo->parent)
                if(taskInfo->parent->curHour==hourNumber)
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
    Q_ASSERT(parentInfo != nullptr);
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
    if (childInfo == nullptr) {
        return QModelIndex();
    }
    Q_ASSERT(childInfo != nullptr);
    TaskInfo* parentInfo = childInfo->parent;
    if (parentInfo != nullptr) {
        return createIndex(findRow(parentInfo), COL_PERIOD, parentInfo);
    }
    else {
        return QModelIndex();
    }
}

int Planner::findRow(const TaskInfo *TaskInfo) const
{
    Q_ASSERT(TaskInfo != nullptr);
    const TaskInfoList& parentInfoChildren = TaskInfo->parent != 0 ? TaskInfo->parent->children: _tasks;
    TaskInfoList::const_iterator position = qFind(parentInfoChildren, *TaskInfo);
    Q_ASSERT(position != parentInfoChildren.end());
    return std::distance(parentInfoChildren.begin(), position);
}

bool Planner::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        const TaskInfo* parentInfo = static_cast<const TaskInfo*>(parent.internalPointer());
        Q_ASSERT(parentInfo != nullptr);
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
    Q_ASSERT(parentInfo != nullptr);
    return !parentInfo->mapped;
}

void Planner::fetchMore(const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid());
    TaskInfo* parentInfo = static_cast<TaskInfo*>(parent.internalPointer());
    Q_ASSERT(parentInfo != nullptr);
    Q_ASSERT(!parentInfo->mapped);
    if (!parentInfo->children.isEmpty())
        parentInfo->mapped=true;

}

bool Planner::saveExcelReport(const QString &fileName)
{
    qDebug()<<"saveExcelReport"<<fileName;
    Document *xlsx= new Document(fileName);
    //qDebug()<<1;
    for(int j=0;j<headers.count();j++){
        //qDebug()<<2;
        xlsx->write(1,j+1,headers.value(headers.keys().at(j)));
    }
    for(int j=0;j<headers.count();j++)
        for(int i=0;i<_tasks.count();i++)
            if(j==Columns::COL_NOTES){
                //qDebug()<<3;
//                xlsx->write(i+2,j+1,lostTimeNoteList.at(data(createIndex(i,j)).toInt()));
                xlsx->write(i+2,j+1,_tasks.at(i).taskNote);
            } else {
                qDebug()<<4<<data(createIndex(i,j),Qt::DisplayRole);
                //xlsx->write(i+2,j+1,data(createIndex(i,j),Qt::DisplayRole));
                //qDebug()<<5;
            }
    qDebug()<<"saveExcelReport"<<"done";
    return xlsx->saveAs(fileName);
}

int Planner::getProgressHour()
{
    //DLE= сумма времени цикла / время × кол-во операторов
    int workContent=0;
    int doneContent=0;
            int planned_stops=0;
    if(!_tasks.isEmpty() && _tasks.count()>lastHour && lastHour!=1){
        for(int j=0;j<_tasks.at(lastHour).children.count();j++){
            if(_tasks.at(lastHour).children.at(j).kanbanObj.countParts==0 &&
                    _tasks.at(lastHour).children.at(j).done)
                planned_stops+=_tasks.at(lastHour).children.at(j).taskWorkContent;
            if(_tasks.at(lastHour).children.at(j).done)
                doneContent+=_tasks.at(lastHour).children.at(j).kanbanObj.countParts;
        }
        workContent=(3600-planned_stops)/avgWorkContent;
    }
    if(workContent==0)
        return 0;
    return doneContent*100/workContent;
}

int Planner::getProgressShift()
{
    int workContent=0;
    int doneContent=0;
    for(int i=0;i<_tasks.count();i++){
            int planned_stops=0;
        for(int j=0;j<_tasks[i].children.count();j++){
            if(_tasks[i].children.at(j).kanbanObj.countParts==0 &&
                    _tasks[i].children.at(j).done)
                planned_stops+=_tasks[i].children.at(j).taskWorkContent;
            if(_tasks[i].children[j].done)
                //doneContent+=_tasks[i].children[j].kanbanObj.countParts;
                doneContent+=_tasks[i].children[j].taskWorkContent*
                        _tasks[i].children[j].kanbanObj.countParts;
        }
        //workContent+=(3600-planned_stops)/avgWorkContent;
        workContent+=(3600-planned_stops);
    }
//    qDebug()<<2;
//    qDebug()<<doneContent<<workContent;
    if(workContent==0)
        return 0;
//    qDebug()<<3<<"doneContent"<<doneContent;
    return doneContent*100/workContent;

//    qDebug()<<4;
}

void Planner::taskDone(TaskInfo* taskInfo)
{
    qDebug()<<"taskDone start"<<"taskInfo->curHour"<<taskInfo->curHour;
    taskInfo->done=true;
    taskInfo->canceled=false;
    TaskInfo* taskParent = static_cast<TaskInfo*>(taskInfo->parent);
        if(taskParent)
            if(taskParent->curHour!=getCurrentHourNum()){
                if (_tasks.count()>getCurrentHourNum()){
                    qDebug()<<"removeOne";
                    taskParent->children.removeOne(*taskInfo);
                   _tasks[getCurrentHourNum()].children.append(*taskInfo);
                }
            }
    planBoardUpdate();
    qDebug()<<"taskDone finish";
}

void Planner::taskCancel(TaskInfo* taskInfo)
{
    taskInfo->done=false;
    taskInfo->canceled=true;
    //_tasks[taskInfo->curHour].children.removeOne(taskInfo);
    planBoardUpdate();
}

bool Planner::kanbanProdused(const QByteArray &kanban)
{
    qDebug()<<kanban;
    if(kanban=="startSMED "){
        startSMED();
        return false;
    }
    if(kanban=="finishSMED"){
        finishSMED();
        return false;
    }
    for(auto task=_tasks.begin();task!=_tasks.end();++task)
        for(auto child=task->children.begin();child!=task->children.end();++child)
            if(!child->done && !child->canceled && child->kanbanObj.kanban==kanban){
                taskDone(child);
                return true;
            }
    TaskInfo task = TaskInfo();
    if (kanbanMap.contains(kanban))
        task.kanbanObj=kanbanMap.value(kanban);
    task.taskWorkContent=task.kanbanObj.partWorkContent*task.kanbanObj.countParts;
    task.done=true;
    qDebug()<<"&_tasks[getCurrentHourNum()]";
    task.parent=&_tasks[getCurrentHourNum()];
    _tasks[getCurrentHourNum()].children.append(task);
    qDebug()<<"&_tasks[getCurrentHourNum()] fin";
    this->planBoardUpdate();

    return true;
}
