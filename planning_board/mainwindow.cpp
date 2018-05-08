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
        this->setGeometry(50,50,950,600);
    } else {
        QApplication::setOverrideCursor(Qt::BlankCursor);
        this->setWindowState(Qt::WindowFullScreen);
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    }
    this->setWindowTitle("Доска планирования производства");
    tableView = new QTableView(this);
    tableView->setModel(model);
    this->setCentralWidget(tableView);
    this->setFont(QFont("Helvetica [Croyx]",18));
    tableView->show();
    QObject::connect(model,&QAbstractTableModel::dataChanged,[=](){
        tableView->resizeColumnsToContents();
        tableView->resizeRowsToContents();
    });
    tableView->verticalHeader()->setSectionResizeMode (QHeaderView::Stretch);
//    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_PERIOD,QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_PLAN,QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_ACTUAL,QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_REFERENCE,QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_LOSTTIME,QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_NOTES,QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(Planner::Columns::COL_SCRAP,QHeaderView::Fixed);
    //tableView->setColumnWidth(Planner::Columns::COL_PERIOD,250);
    tableView->setColumnWidth(Planner::Columns::COL_PLAN,60);
    tableView->setColumnWidth(Planner::Columns::COL_ACTUAL,60);
    tableView->setColumnWidth(Planner::Columns::COL_SCRAP,60);
    tableView->horizontalHeader()->setSectionsClickable(false);
    tableView->horizontalHeader()->setDefaultSectionSize(100);
    //tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ComboBoxDelegate *comboDelegate = new ComboBoxDelegate(this);
    tableView->setItemDelegateForColumn(Planner::Columns::COL_NOTES,comboDelegate);
    SpinBoxDelegate *spin1Delegate = new SpinBoxDelegate(this);
    tableView->setItemDelegateForColumn(Planner::Columns::COL_LOSTTIME,spin1Delegate);
    SpinBoxDelegate *spin2Delegate = new SpinBoxDelegate(this);
    tableView->setItemDelegateForColumn(Planner::Columns::COL_SCRAP,spin2Delegate);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
//    tableView->resizeColumnsToContents();
//    tableView->resizeRowsToContents();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleView()
{
    tableView->setColumnHidden(3,!tableView->isColumnHidden(3));
    tableView->setColumnHidden(4,!tableView->isColumnHidden(4));
    tableView->setColumnHidden(6,!tableView->isColumnHidden(6));
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void MainWindow::setRowView(int row)
{
    tableView->setColumnHidden(3,!tableView->isColumnHidden(3));
    tableView->setColumnHidden(4,!tableView->isColumnHidden(4));
    tableView->setColumnHidden(6,!tableView->isColumnHidden(6));
    tableView->resizeColumnsToContents();
    tableView->resizeRowToContents(row);
}


