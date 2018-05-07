#ifndef Planner_H
#define Planner_H

#include <QAbstractTableModel>
#include <QMap>


#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE


struct kanbanItem
{
    int countParts;
    int workContent;
    QByteArray reference;
    QByteArray sebango;
    QByteArray kanban;
    kanbanItem(){
        countParts=0;
        workContent=0;
        reference="";
        sebango="";
        kanban="";
    }
};

struct planItem
{
    kanbanItem kanban;
    int countScrap;
    bool done; //fake
    QString scrapNotes;

    planItem(kanbanItem extKanban=kanbanItem()){
        kanban=extKanban;
        countScrap=0;
        done=false;
        scrapNotes="";
    }
};

struct hourItem
{
    QList<planItem> houtPlan;
    int lostTime;
    QString lostTimeNotes;
    hourItem(){
        houtPlan=QList<planItem>();
        lostTime=0;
        lostTimeNotes="";
    }
};


class Planner : public QAbstractTableModel
{
    Q_OBJECT
public:
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
    void addPlan(const planItem &plan);

signals:
//    planChanged(QList<int> plan);
    void editCompleted(const QString &);


private slots:
    void shiftReset();

private:

    QString m_gridData[9][7];  //holds text entered into QTableView
    QMap<QByteArray,kanbanItem> kanbanMap;
    QList<hourItem> planBoard;

};

#endif // Planner_H
