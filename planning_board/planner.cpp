#include "planner.h"
#include <QTime>
#include <QDebug>


Planner::Planner(QObject *parent) : QAbstractTableModel(parent)
{
    this->readExcelData();
    QTimer *hourTimer = new QTimer(this);
    QObject::connect(tableTimer,&QTimer::timeout,[hourTimer,this](){
        QTime ct = QTime::currentTime();
        hourTimer->start(qMax(3600000-2000,ct.msecsTo(QTime(ct.hour(),59,59))+1000));
        if(ct.hour()==0||ct.hour()==6||ct.hour()==15)
            this->shiftReset();
        else
            this->planUpdate();
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
    while (!xlsx->read(i,1).toByteArray().isEmpty()){
        if(xlsx->read(i,2).toInt()<3600){
            kanbanItem kanbanItm;
            kanbanItm.kanban=xlsx->read(i,1).toByteArray();
            kanbanItm.workContent=xlsx->read(i,2).toInt();
            kanbanItm.countParts=xlsx->read(i,3).toByteArray();
            kanbanItm.reference=xlsx->read(i,4).toByteArray();
            kanbanItm.sebango=xlsx->read(i,5).toByteArray();
            kanbanMap.insert(kanban,kanbanItm);
        }
        i++;
    }
    return true;
}

void Planner::addKanban(const QByteArray &kanban)
{
    if(kanbanMap.contains(kanban))
        addPlan(planItem(kanbanMap.value(kanban)));
}

void Planner::addPlan(const planItem &plan)
{
    QTime ct=QTime::currentTime();
    int hourValue=3600-ct.minute()*60+ct.second();
    int reqWC;
    int hourNumber;
    if(ct.hour()<6)
        hourNumber=ct.hour();
    else if (ct.hour()<15)
            hourNumber=ct.hour()-6;
        else
            hourNumber=ct.hour()-15;
    // update planBoard acording done
    reqWC=0;

    for(int i;i<planBoard.at(hourNumber).houtPlan.count();++i)
        reqWC=+planBoard.at(hourNumber).houtPlan.at(i).kanban.workContent;
    //if(hourValue>reqWC)

    for(int i=hourNumber;i<planBoard.count();++i){
        reqWC = plan.kanban.workContent*plan.kanban.countParts;
        if(reqWC<hourValue){
            hourValue-=reqWC;
            plan.at(i).hourNumber=hourNumber;
        }
        hourNumber++;
        hourValue=3600;
    }
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
}

QVariant Planner::headerData(int section, Qt::Orientation orientation, int role) const
{

    switch(role){
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal) {
            QStringList headers;
            headers << "Период" <<  "План" <<  "Факт"<<  "Референс"<<  "Потерянное\nвремя" <<  "Замечания" << "Брак";
            if(section>=0 && section<=6)
                return headers.at(section);
        }
        break;
    case Qt::FontRole:
        return QFont("Arial",12);
        break;
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



    switch(role){
    case Qt::DisplayRole: {
            int itemsCount=9;
            int startHour;
            QTime ct = QTime::currentTime();
            if(ct.hour()<6) {
                itemsCount=6;
                startHour=0;
            }
            else if (ct.hour()<15)
                    startHour=6;
                else
                    startHour=15;

            if(row<itemsCount){
                switch(col){
                    case 0:
                        return QString("%1.00-%2.00").arg(startHour+row).arg((startHour+row+1)%24);
                    default:
                        return 0;
                }
            } else {
                return QVariant();
            }
        break;
    }
    case Qt::FontRole:
            return QFont("Helvetica [Croyx]",30);
        break;
    case Qt::BackgroundRole:
        //QBrush redBackground(Qt::red);
        //return redBackground;
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    //case Qt::CheckStateRole:
        //return Qt::Checked;
    }

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
    if(index.column()==3)
        return QAbstractTableModel::flags(index) & ~Qt::ItemIsEditable;
    else
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

void Planner::shiftReset()
{
    //save to shift report to Excel

    plan.clear();

//    for(int i=0;i<this->rowCount();++i) {
//        plan.at(i).countActual=0;
//        plan.at(i).countPlan=0;
//        plan.at(i).countScrap=0;
//        plan.at(i).lostTime=0;
//        plan.at(i).notes="";
//        plan.at(i).reference=0;
//        plan.at(i).=0;
//        plan.at(i).kanban="";
//    }


    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
}
