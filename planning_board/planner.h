#ifndef Planner_H
#define Planner_H

#include <QAbstractItemModel>
#include <QVector>
#include "taskinfo.h"

#include "xlsxdocument.h"

QTXLSX_USE_NAMESPACE



class Planner : public QAbstractItemModel
{
    Q_OBJECT





public:

    enum ServiceCommands
    {
        SC_NONE,
        SC_TOGGLE_VIEW,
        SC_SCRAP_LAST_HOUR,
        SC_SCRAP_CUR_HOUR,
        SC_NOTE_LOST,
        SC_NOTE_SCRAP
    };
    Q_ENUM(ServiceCommands)


    enum Columns
    {
        COL_PERIOD,
        COL_PLAN,
        COL_ACTUAL,
        COL_SEBANGO,
        COL_LOSTTIME,
        COL_NOTES,
        COL_SCRAP,
        COL_OPERATORS,
        COL_STATUS
    };
    Q_ENUM(Columns)

    explicit Planner(QObject *parent=0);
    ~Planner();


    QVariant headerData(int section, Qt::Orientation orientation, int role) const override ;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);
    bool hasChildren(const QModelIndex &parent) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    bool readExcelData(const QString &fileName="planning_data.xlsx");
    Qt::ItemFlags flags(const QModelIndex & index) const override ;
    int getProgress();
    int getCurrentHourNum() const;
    int getStartHourNum() const;
    virtual void keyboardSearch(const QString& search) {}


public slots:
    void parseBuffer(const QByteArray &kanban);
    void addKanban(const QByteArray &kanban);
    bool kanbanProdused(const QByteArray &kanban);


private:

    void startNextShift();
    void addKnownTask(const QByteArray &sebango,int hourNum,int workContent);
    void startSMED();
    void finishSMED();    
    //struct TaskInfo;
    typedef QVector<TaskInfo> TaskInfoList;
    TaskInfoList _tasks;
    TaskInfoList _notAttachedTasks;
    int lastHour = -1;
    int avgWorkContent=100;

    QMap<QByteArray,kanbanItem> kanbanMap;
    QMap<Columns,QString> headers;
    QStringList taskNoteList;//=QStringList()<<""<<"Организационные \nпроблемы"<<"Нет тары"<<"Нет компонентов"
    //              <<"Доработка"<<"Поломка робота"<<"Поломка тетра"<<"Поломка другое";
    //QStringList actionsNoteList=QStringList()<<""<<"Готово"<<"Нет на литье"<<"Нет тары"<<"Нет BOP"<<"Отмена";
    QStringList statusList=QStringList()<<""<<"Готово"<<"Отмена"; //1-готово 2-отмена
    //QStringList ResultsNoteList=QStringList()<<""<<;
    QStringList scrapNoteList;//=QStringList()<<""<<"Царапины"<<"Прожог";
    //QStringList serviceCommandsList=QStringList()<<"SCRAP"<<"LNOTE"<<"SNOTE";

    void planBoardUpdate();
    bool saveExcelReport(const QString &fileName);
    int findRow(const TaskInfo *TaskInfo) const;
    void taskDone(TaskInfo* taskInfo);
    void taskCancel(TaskInfo* taskInfo);



signals:
    void editCompleted(const QString &);
    void modelSpanned(int,int,int,int);
    void hourHasChanged(int);
    void uiEnable(bool);


};

#endif // Planner_H
