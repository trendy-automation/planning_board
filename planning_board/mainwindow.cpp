#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
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
    this->setFont(QFont("Helvetica [Croyx]",FONT_VALUE));
    treeView->show();
    statusLabel = new QLabel(this);
    statusProgressBar = new QProgressBar(this);
    //statusLabel->setText("Электронная доска планирования");
    statusLabel->setText("Выполнение плана");
    //statusProgressBar->setTextVisible(false);
    ui->statusBar->addPermanentWidget(statusLabel);
    ui->statusBar->addPermanentWidget(statusProgressBar,1);
    ui->statusBar->showMessage("Faurecia");
    statusProgressBar->setValue(0);
    //treeView->keyboardSearch();
    treeView->setTabKeyNavigation(false);
    Planner *planner = static_cast<Planner*>(model);
    QObject::connect(planner,&Planner::dataChanged,[planner,this](){
//                treeView->resizeColumnsToContents();
//                treeView->resizeRowsToContents();
        statusProgressBar->setValue(planner->getProgress());
    });
    //QObject::connect(planner,&Planner::modelSpanned,treeView,&QTreeView     ::setSpan);
    //treeView     ->verticalHeader()->setSectionResizeMode (QHeaderView::ResizeToContents);
    //treeView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_PERIOD,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_PLAN,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_ACTUAL,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_REFERENCE,QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_LOSTTIME,QHeaderView::Fixed);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_NOTES,QHeaderView::Stretch);
    treeView->header()->setSectionResizeMode(Planner::Columns::COL_SCRAP,QHeaderView::Fixed);
    treeView->header()->setSectionsClickable(false);
    treeView->header()->setDefaultSectionSize(DefaultSectionSize);
    treeView->setColumnWidth(Planner::Columns::COL_PERIOD,COL_PERIOD_SIZE);
    treeView->setColumnWidth(Planner::Columns::COL_LOSTTIME,COL_LOSTTIME_SIZE);
    treeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    treeView->setItemDelegateForColumn(Planner::Columns::COL_NOTES, new ComboBoxDelegate(treeView));
//    treeView->setItemDelegateForColumn(Planner::Columns::COL_ACTUAL,new SpinBoxDelegate); //TBD
    treeView->setItemDelegateForColumn(Planner::Columns::COL_LOSTTIME,new SpinBoxDelegate);
    treeView->setItemDelegateForColumn(Planner::Columns::COL_SCRAP,new SpinBoxDelegate);
    treeView->setItemDelegateForColumn(Planner::Columns::COL_OPERATORS,new SpinBoxDelegate);
//    treeView->setStyleSheet("QTreeView::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    losttimeDelegate->setStyleSheet("SpinBoxDelegate::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    scrapDelegate->setStyleSheet("SpinBoxDelegate::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
//    nodesDelegate->setStyleSheet("ComboBoxDelegate::item { border: 0.5px ; border-style: solid ; border-color: lightgray ;}");
    treeView->setSelectionMode(QAbstractItemView::NoSelection);//SingleSelection
    treeView->setSelectionBehavior(QAbstractItemView::SelectItems);//SelectItems
    //treeView->resizeColumnsToContents();
    //treeView->resizeRowsToContents();
    treeView->setWordWrap(true);
    treeView->setTextElideMode(Qt::ElideMiddle);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleView()
{
    treeView->setColumnHidden(Planner::Columns::COL_ACTUAL,!treeView->isColumnHidden(Planner::Columns::COL_ACTUAL));
    treeView->setColumnHidden(Planner::Columns::COL_REFERENCE,!treeView->isColumnHidden(Planner::Columns::COL_REFERENCE));
    treeView->setColumnHidden(Planner::Columns::COL_SCRAP,!treeView->isColumnHidden(Planner::Columns::COL_SCRAP));
//    treeView->resizeColumnsToContents();
//    treeView->resizeRowsToContents();
}

void MainWindow::setRowView(int row)
{
    treeView->setColumnHidden(Planner::Columns::COL_ACTUAL,!treeView->isColumnHidden(Planner::Columns::COL_ACTUAL));
    treeView->setColumnHidden(Planner::Columns::COL_REFERENCE,!treeView->isColumnHidden(Planner::Columns::COL_REFERENCE));
    treeView->setColumnHidden(Planner::Columns::COL_SCRAP,!treeView->isColumnHidden(Planner::Columns::COL_SCRAP));
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
