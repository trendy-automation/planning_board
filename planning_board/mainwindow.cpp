#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QAction>
#include <QWidgetAction>
#include <QGridLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include "planner.h"

MainWindow::MainWindow(QAbstractItemModel *model, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (QApplication::applicationDirPath().toLower().contains("build")) {
        this->setGeometry(100,30,800,700);
    } else {
        QApplication::setOverrideCursor(Qt::BlankCursor);
        this->setWindowState(Qt::WindowFullScreen);
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    }
    this->setWindowTitle("Доска планирования производства");
    treeView = new QTreeView(this);
    treeView->setModel(model);
    this->setCentralWidget(treeView);
    this->setFont(QFont(FONT_TYPE,FONT_VALUE));
    treeView->show();
    statusLabelHour = new QLabel(this);
    statusLabelShift = new QLabel(this);
    statusProgressBarHour = new QProgressBar(this);
    statusProgressBarHour->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Maximum);
    statusProgressBarShift = new QProgressBar(this);
    statusProgressBarShift->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Maximum);
    statusLabelHour->setText("Выполнение плана за текущий час");
    statusLabelShift->setText("Выполнение плана за смену");
    QGroupBox *progressBox= new QGroupBox(this);
    QGridLayout *layout= new QGridLayout(this);
    layout->addWidget(statusLabelHour,0,0);
    layout->addWidget(statusProgressBarHour,0,1);
    layout->addWidget(statusLabelShift,1,0);
    layout->addWidget(statusProgressBarShift,1,1);
    layout->setContentsMargins(0,0,0,0);
    progressBox->setLayout(layout);
    ui->statusBar->setContentsMargins(0,0,0,0);
    ui->statusBar->addPermanentWidget(progressBox,1);
    statusProgressBarHour->setValue(0);
    statusProgressBarShift->setValue(0);
    //treeView->keyboardSearch();
    treeView->setTabKeyNavigation(false);
    Planner *planner = static_cast<Planner*>(model);
    QObject::connect(planner,&Planner::dataChanged,[planner,this](){
//                treeView->resizeColumnsToContents();
//                treeView->resizeRowsToContents();
        statusProgressBarHour->setValue(planner->getProgressHour());
        statusProgressBarShift->setValue(planner->getProgressShift());
    });
//    QObject::connect(planner,&Planner::hourHasChanged,[this,model]( int newHour){
//        qDebug()<<"hourHasChanged form"<<newHour-1<<"to"<<newHour;
//        //treeView->collapse(model->index(0,newHour - 1));
//        //treeView->expand(model->index(0,newHour));
//        //qDebug()<<"hourHasChanged done";
//    });

    QObject::connect(planner,&Planner::uiEnable,[this](bool enable){
        treeView->setUpdatesEnabled(enable);
    });




    this->ui->menuBar->setStyleSheet("QMenuBar{padding: 1px 10px;"
                                     "background: transparent;"
                                     "border: 0.5px ; border-style: solid ; "
                                     "border-color: lightgray ; border-radius: 4px;"
                                     "}"
                                     "QMenuBar::item{padding: 1px 10px;"
                                     "background: transparent;"
                                     "margin: 3px ;"
                                     "border: 0.7px ; border-style: solid ; "
                                     "border-color: black ; border-radius: 4px;"
                                     "}"
                                     "QMenuBar::item:pressed {"
                                     "background: orange;"
                                     "color:blue;"
                                     "}");
    this->ui->menuBar->setFont(this->font());

    QWidget *frame = new QWidget(this);
    frame->setGeometry(0, 0, 141, 51);
    frame->setStyleSheet("background-image: url(:/logo.png)");
    //frame->setStyleSheet("background-image: url(:/logo.jpg)");
    //frame->setStyleSheet(QString("background-image: url(%1/logo.jpg)").arg(qApp->applicationDirPath()));
    QAction *logoPlace = this->ui->menuBar->addAction("             ");
    QAction *action1 = this->ui->menuBar->addAction("Развернуть текущий час");
    this->connect(action1, &QAction::triggered, [this,model,planner,action1](){
        QModelIndex index = model->index(planner->getCurrentHourNum(),0);
        if(action1->property("isExpanded").toBool()){
            //treeView->setExpanded(model->index(0, planner->getCurrentHourNum()),false);
            //TODO check is index achivable
            treeView->collapse(index);
            action1->setText("Развернуть текущий час");
            action1->setProperty("isExpanded",false);
        } else {
            //treeView->setExpanded(model->index(0, planner->getCurrentHourNum()),true);
            treeView->expand(index);
            action1->setText("Свернуть текущий час   ");
            action1->setProperty("isExpanded",true);
        }
    });

//    QObject::connect(treeView, &QTreeView::clicked, [this,planner,action1](const QModelIndex &index){
//            if (index.row()==planner->getCurrentHourNum())
//                action1->trigger();
//            else
//                treeView->isExpanded(index)? treeView->collapse(index) : treeView->expand(index);
//        }
//    );

    QAction *action2 = this->ui->menuBar->addAction("Развернуть все часы");

    this->connect(action2, &QAction::triggered, [this,model,action1,action2](){
        if(action2->property("isExpanded").toBool()){
            treeView->collapseAll();
            action1->setText("Развернуть текущий час");
            action1->setProperty("isExpanded",false);
            action2->setText("Развернуть все часы");
            action2->setProperty("isExpanded",false);
        } else {
            treeView->expandAll();
            action1->setText("Свернуть текущий час   ");
            action1->setProperty("isExpanded",true);
            action2->setText("Свернуть все часы   ");
            action2->setProperty("isExpanded",true);
        }
    });

    //QObject::connect(planner,&Planner::modelSpanned,treeView,&QTreeView     ::setSpan);
    //treeView     ->verticalHeader()->setSectionResizeMode (QHeaderView::ResizeToContents);
    //treeView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_PERIOD,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_PLAN,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_ACTUAL,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_DELTA,QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_SEBANGO,QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_LOSTTIME,QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_NOTES,QHeaderView::Stretch);
//    treeView->header()->setSectionResizeMode(Planner::Columns::COL_SCRAP,QHeaderView::Fixed);
//    treeView->header()->setSectionResizeMode(Planner::Columns::COL_OPERATORS,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_STATUS,QHeaderView::Fixed);
    treeView->header()->setSectionsClickable(false);
    treeView->header()->setDefaultSectionSize(DefaultSectionSize);
    treeView->setColumnWidth(Planner::Columns::COL_PERIOD,COL_PERIOD_SIZE);
    treeView->setColumnWidth(Planner::Columns::COL_ACTUAL,COL_ACTUAL_SIZE);
    treeView->setColumnWidth(Planner::Columns::COL_LOSTTIME,COL_LOSTTIME_SIZE);
//    treeView->setColumnWidth(Planner::Columns::COL_OPERATORS,COL_OPERATORS_SIZE);
    treeView->setColumnWidth(Planner::Columns::COL_STATUS,COL_STATUS_SIZE);
    treeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //treeView->setMinimumHeight(500);
    //treeView->setMaximumHeight(1000);
    treeView->setItemDelegate(new GridDelegate(treeView));

    QStringList taskNoteList = planner->property("taskNoteList").toStringList();
    QStringList statusList = planner->property("statusList").toStringList();
//    //QStringList scrapNoteList = treeView->model()->property("scrapNoteList").toStringList(); //TBD

    treeView->setItemDelegateForColumn(Planner::Columns::COL_NOTES, new ComboBoxDelegate(this,taskNoteList));
    treeView->setItemDelegateForColumn(Planner::Columns::COL_PERIOD,new StyledItemDelegate(this));
    treeView->setItemDelegateForColumn(Planner::Columns::COL_SEBANGO,new StyledItemDelegate(this));
//    treeView->setItemDelegateForColumn(Planner::Columns::COL_LOSTTIME,new SpinBoxDelegate(this,0,80));
//    treeView->setItemDelegateForColumn(Planner::Columns::COL_SCRAP,new SpinBoxDelegate(this,0,1000));
//    treeView->setItemDelegateForColumn(Planner::Columns::COL_OPERATORS,new SpinBoxDelegate(this,1,4));
    treeView->setItemDelegateForColumn(Planner::Columns::COL_STATUS,new ComboBoxDelegate(this,statusList));
    treeView->setSelectionMode(QAbstractItemView::NoSelection);//SingleSelection
    treeView->setSelectionBehavior(QAbstractItemView::SelectItems);//SelectItems
    treeView->setWordWrap(true);
    treeView->setTextElideMode(Qt::ElideMiddle);

//    //    treeView->setItemDelegateForColumn(Planner::Columns::COL_ACTUAL,new SpinBoxDelegate); //TBD
//    //    treeView->setStyleSheet("QTreeView::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    //    losttimeDelegate->setStyleSheet("SpinBoxDelegate::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    //    scrapDelegate->setStyleSheet("SpinBoxDelegate::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    //    nodesDelegate->setStyleSheet("ComboBoxDelegate::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    //treeView->resizeColumnsToContents();
//    //treeView->resizeRowsToContents();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleView()
{
    treeView->setColumnHidden(Planner::Columns::COL_ACTUAL,!treeView->isColumnHidden(Planner::Columns::COL_ACTUAL));
    treeView->setColumnHidden(Planner::Columns::COL_SEBANGO,!treeView->isColumnHidden(Planner::Columns::COL_SEBANGO));
//    treeView->setColumnHidden(Planner::Columns::COL_SCRAP,!treeView->isColumnHidden(Planner::Columns::COL_SCRAP));
//    treeView->resizeColumnsToContents();
//    treeView->resizeRowsToContents();
}

void MainWindow::setRowView(int row)
{
    treeView->setColumnHidden(Planner::Columns::COL_ACTUAL,!treeView->isColumnHidden(Planner::Columns::COL_ACTUAL));
    treeView->setColumnHidden(Planner::Columns::COL_SEBANGO,!treeView->isColumnHidden(Planner::Columns::COL_SEBANGO));
//    treeView->setColumnHidden(Planner::Columns::COL_SCRAP,!treeView->isColumnHidden(Planner::Columns::COL_SCRAP));
//    treeView->resizeColumnsToContents();
//    treeView->resizeRowToContents(row);
}


/*
  QString style = "QTreeWidget::item:!selected "
    "{ "
      "border: 1px solid gainsboro; "
      "border-left: none; "
      "border-top: none; "
    "}"
    "QTreeWidget::item:selected {}";
  allergiesListView->setStyleSheet(style);
*/

