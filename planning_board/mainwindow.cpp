#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include "planner.h"

MainWindow::MainWindow(QAbstractTableModel *model, QWidget *parent) :
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
    tableView = new QTableView(this);
    tableView->setModel(model);
    this->setCentralWidget(tableView);
    this->setFont(QFont("Helvetica [Croyx]",FONT_VALUE));
    tableView->show();
    QObject::connect(model,&QAbstractTableModel::dataChanged,[=](){
//                tableView->resizeColumnsToContents();
//                tableView->resizeRowsToContents();
    });
    Planner *planner = static_cast<Planner*>(model);
    QObject::connect(planner,&Planner::modelSpanned,tableView,&QTableView::setSpan);
    tableView->verticalHeader()->setSectionResizeMode (QHeaderView::ResizeToContents);
    //tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_PERIOD,QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_PLAN,QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_ACTUAL,QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_REFERENCE,QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_LOSTTIME,QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_NOTES,QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_SCRAP,QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionsClickable(false);
    tableView->horizontalHeader()->setDefaultSectionSize(DefaultSectionSize);
    tableView->setColumnWidth(Planner::Columns::COL_PERIOD,COL_PERIOD_SIZE);
    tableView->setColumnWidth(Planner::Columns::COL_LOSTTIME,COL_LOSTTIME_SIZE);
    tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    tableView->setItemDelegateForColumn(Planner::Columns::COL_NOTES, new ComboBoxDelegate(tableView));
//    tableView->setItemDelegateForColumn(Planner::Columns::COL_ACTUAL,new SpinBoxDelegate); //TBD
    tableView->setItemDelegateForColumn(Planner::Columns::COL_LOSTTIME,new SpinBoxDelegate);
    tableView->setItemDelegateForColumn(Planner::Columns::COL_SCRAP,new SpinBoxDelegate);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    //tableView->resizeColumnsToContents();
    //tableView->resizeRowsToContents();
    tableView->setWordWrap(true);
    tableView->setTextElideMode(Qt::ElideMiddle);

    statusLabel = new QLabel(this);
    statusProgressBar = new QProgressBar(this);
    //statusLabel->setText("Электронная доска планирования");
    statusLabel->setText("Выполнение плана");
    //statusProgressBar->setTextVisible(false);
    ui->statusBar->addPermanentWidget(statusLabel);
    ui->statusBar->addPermanentWidget(statusProgressBar,1);
    ui->statusBar->showMessage("Faurecia");
    statusProgressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleView()
{
    tableView->setColumnHidden(Planner::Columns::COL_ACTUAL,!tableView->isColumnHidden(Planner::Columns::COL_ACTUAL));
    tableView->setColumnHidden(Planner::Columns::COL_REFERENCE,!tableView->isColumnHidden(Planner::Columns::COL_REFERENCE));
    tableView->setColumnHidden(Planner::Columns::COL_SCRAP,!tableView->isColumnHidden(Planner::Columns::COL_SCRAP));
//    tableView->resizeColumnsToContents();
//    tableView->resizeRowsToContents();
}

void MainWindow::setRowView(int row)
{
    tableView->setColumnHidden(Planner::Columns::COL_ACTUAL,!tableView->isColumnHidden(Planner::Columns::COL_ACTUAL));
    tableView->setColumnHidden(Planner::Columns::COL_REFERENCE,!tableView->isColumnHidden(Planner::Columns::COL_REFERENCE));
    tableView->setColumnHidden(Planner::Columns::COL_SCRAP,!tableView->isColumnHidden(Planner::Columns::COL_SCRAP));
//    tableView->resizeColumnsToContents();
//    tableView->resizeRowToContents(row);
}


