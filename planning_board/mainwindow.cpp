#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
//#include <QTime>

MainWindow::MainWindow(QAbstractTableModel *model, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (QApplication::applicationDirPath().toLower().contains("build")) {
        this->setGeometry(50,50,850,600);
    } else {
        QApplication::setOverrideCursor(Qt::BlankCursor);
        this->setWindowState(Qt::WindowFullScreen);
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    }
    this->setWindowTitle("Доска планирования производства");
    tableView = new QTableView(this);
    tableView->setModel(model);
    tableView->show();
    this->setCentralWidget(tableView);
//    tableWidget->setVerticalHeaderLabels(QStringList()<<""<<""<<""<<""<<""<<""<<""<<""<<"");
    tableView->setColumnHidden(3,true);
    tableView->setColumnHidden(4,true);
    tableView->setColumnHidden(6,true);

    tableView->setColumnWidth(0,250);
    tableView->horizontalHeader()->setFont(QFont("Arial",30));
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
    tableView->horizontalHeader()->setDefaultSectionSize(100);
    tableView->horizontalHeader()->setSectionsClickable(false);
    tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ComboBoxDelegate *comboDelegate = new ComboBoxDelegate(this);
    tableView->setItemDelegateForColumn(3,comboDelegate);


    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

    tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableView->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleView()
{

}

void MainWindow::toggleRow(int row)
{
    tableView->setColumnHidden(3,!tableView->isColumnHidden(3));
    tableView->setColumnHidden(4,!tableView->isColumnHidden(4));
    tableView->setColumnHidden(6,!tableView->isColumnHidden(6));
    tableView->setRowHeight(row,100); //expand row
    tableView->resizeColumnsToContents();
}



//void MainWindow::updatePlan(QList<int> plan)
//{
//    //qDebug()<<"plan"<<plan;
//    int itemsCount=9;
//    int row;
//    QTime ct = QTime::currentTime();
//    if(ct.hour()<6){
//        row=ct.hour();
//        itemsCount=6;
//        tableWidget->setRowHidden(6,true);
//        tableWidget->setRowHidden(7,true);
//        tableWidget->setRowHidden(8,true);
//    }
//    else {
//        tableWidget->setRowHidden(6,false);
//        tableWidget->setRowHidden(7,false);
//        tableWidget->setRowHidden(8,false);
//        if (ct.hour()<15)
//            row=ct.hour()-6;
//        else
//            row=ct.hour()-15;
//    }
//    //qDebug()<<"row="<<row<<"hour"<<QTime::currentTime().hour()<<"plan"<<plan;
//    while (!plan.isEmpty() && row<itemsCount) {
//        //qDebug()<<"row="<<row;
//        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(plan.takeAt(0))));
//        row++;
//    }

//}



