#ifndef Planner_H
#define Planner_H

#include <QObject>
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

class Planner : public QObject
{
    Q_OBJECT
public:
    explicit Planner(QObject *parent = nullptr);
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
