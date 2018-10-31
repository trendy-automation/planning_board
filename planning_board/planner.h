#ifndef Planner_H
#define Planner_H

#include <QAbstractItemModel>
#include <QVector>

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

class Planner : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum ServiceCommands
    {
        SC_SCRAP,
        SC_NOTE_LOST,
        SC_NOTE_SCRAP
    };
    Q_ENUM(ServiceCommands)


    enum Columns
    {
        COL_PERIOD,
        COL_PLAN,
        COL_ACTUAL,
        COL_REFERENCE,
        COL_LOSTTIME,
        COL_NOTES,
        COL_SCRAP,
        COL_OPERATORS
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
    virtual void keyboardSearch(const QString& search) {}

public slots:
    void parseBuffer(const QByteArray &kanban);
    void addKanban(const QByteArray &kanban);

private:



    struct TaskInfo;
    typedef QVector<TaskInfo> TaskInfoList;
    TaskInfoList _tasks;
    TaskInfoList _notAttachedTasks;
    int lastHour = -1;

    QMap<QByteArray,kanbanItem> kanbanMap;
    QMap<Columns,QString> headers;
    QStringList lostTimeNoteList;//=QStringList()<<""<<"Организационные \nпроблемы"<<"Нет тары"<<"Нет компонентов"
    //              <<"Доработка"<<"Поломка робота"<<"Поломка тетра"<<"Поломка другое";
    QStringList ActionsNoteList=QStringList()<<""<<"Готово"<<"Нет на литье"<<"Нет тары"<<"Нет BOP"<<"Отмена";
    //QStringList ResultsNoteList=QStringList()<<""<<;
    QStringList scrapNoteList;//=QStringList()<<""<<"Царапины"<<"Прожог";
    //QStringList serviceCommandsList=QStringList()<<"SCRAP"<<"LNOTE"<<"SNOTE";

    void planBoardUpdate();
    int getCurrentHourNum() const;
    int getStartHourNum() const;
    bool saveExcelReport(const QString &fileName);
    int findRow(const TaskInfo *TaskInfo) const;

signals:
    void editCompleted(const QString &);
    void modelSpanned(int,int,int,int);


};

#endif // Planner_H
