#ifndef Planner_H
#define Planner_H

#include <QAbstractTableModel>
#include <QMap>


#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE


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

class taskItem
{
public:
    taskItem(kanbanItem kanban=kanbanItem()){
        kanbanObj=kanban;
        countScrap=0;
        done=false;
        scrapNotes="";
    }
    void setDone(){
        done=true;
    }
    kanbanItem kanbanObj;
    int taskWorkContent;
    int countScrap;
    bool done;
    QString scrapNotes;
};

class hourItem
{
public:
    hourItem(){
        hourPlan=QList<taskItem*>();
        lostTime=0;
        lostTimeNotes="";
    }
    void appendTask(taskItem *task){
        hourPlan.append(task);
    }
    QList<taskItem*> hourPlan;
    int lostTime;
    QString lostTimeNotes;
};


class Planner : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns
    {
        COL_PERIOD,
        COL_PLAN,
        COL_ACTUAL,
        COL_REFERENCE,
        COL_LOSTTIME,
        COL_NOTES,
        COL_SCRAP
    };
    Q_ENUM(Columns)
    explicit Planner(QObject *parent=0);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override ;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool readExcelData(const QString &fileName="work_content.xlsx");
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override ;

public slots:
    void addKanban(const QByteArray &kanban);

private:
    void planBoardUpdate();

signals:
    void editCompleted(const QString &);

private slots:

private:

    QMap<QByteArray,kanbanItem> kanbanMap;
    QList<hourItem*> planBoard;
    QList<taskItem*> tasks;
    QStringList headers=QStringList() << "Период" <<  "План" <<  "Факт"<<  "Референс"<<
                                         "Потерянное\nвремя" <<  "Замечания" << "Брак";

};

#endif // Planner_H
