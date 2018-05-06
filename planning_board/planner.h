#ifndef Planner_H
#define Planner_H

#include <QAbstractTableModel>
#include <QAbstractItemView>
#include <QMap>
#include "ComboBoxDelegate.h"


#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE

struct planItem
{
    int number;
    int countPlan;
    int countActual;
    QByteArray reference;
    int lostTime;
    QString notes;
    int countScrap;

    int workContent;
    QByteArray kanban;
    QByteArray sebango;
};

struct partItem
{
    int workContent;
    QByteArray kanban;
};

class Planner : public QAbstractTableModel
{
    Q_OBJECT
public:
    Planner(QObject *parent=0);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override ;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    explicit Planner(QObject *parent = nullptr);
    bool readExcelData(const QString &fileName="work_content.xlsx");
    void addPlan(const QByteArray &kanban);
//    QList<int> getPlan();
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override ;
private:
    QString m_gridData[9][7];  //holds text entered into QTableView
    QMap<QByteArray,int> workContentMap;
    //QList<task> plan;
    //QList<task> curentHourPlan;

signals:
//    planChanged(QList<int> plan);
    void editCompleted(const QString &);


public slots:
    void timerHit();
};

#endif // Planner_H
