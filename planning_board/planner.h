#ifndef Planner_H
#define Planner_H

#include <QAbstractTableModel>
#include <QMap>

#include "xlsxdocument.h"
QTXLSX_USE_NAMESPACE
//#include "xlsxworkbook.h"
//#include "xlsxworksheet.h"

//template <typename T>
struct task
{
    QByteArray kanban;
    int workContent;
};

class Planner : public QAbstractTableModel
{
    Q_OBJECT
public:
    Planner(QObject *parent=0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    explicit Planner(QObject *parent = nullptr);
    bool readExcelData(const QString &fileName="work_content.xlsx");
    void addPlan(const QByteArray &kanban);
    QList<int> getPlan();

private:
    QMap<QByteArray,int> workContentMap;
    QList<task> plan;
    //QList<task> curentHourPlan;

signals:
    planChanged(QList<int> plan);

public slots:
};

#endif // Planner_H
