#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QHeaderView>
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
//    tableWidget = new QTableWidget(9, 4, this);
//    QStringList headers;
//    headers << "Период" <<  "План" <<  "Факт"<<  "Референс"<<  "Потерянное\nвремя" <<  "Замечания" << "Брак";
//    tableWidget->setHorizontalHeaderLabels(headers);
//    tableWidget->setVerticalHeaderLabels(QStringList()<<""<<""<<""<<""<<""<<""<<""<<""<<"");
//    tableWidget->setColumnHidden(3,true);
//    tableWidget->setColumnHidden(4,true);
//    tableWidget->setColumnHidden(6,true);

    //QGridLayout * gridlay = new QGridLayout;
    //tableWidget->setLayout(gridlay);
//    this->setCentralWidget(tableWidget);
//    tableWidget->setColumnWidth(0,250);
//    tableWidget->horizontalHeader()->setFont(QFont("Arial",30));
//    tableWidget->horizontalHeader()->setStretchLastSection(true);
//    tableWidget->verticalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
//    //tableWidget->horizontalHeader()->setDefaultSectionSize(100);
//    tableWidget->horizontalHeader()->setSectionsClickable(false);
//    tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
//    tableWidget->setFont(QFont("Helvetica [Croyx]",30));
//    ComboBoxDelegate *comboDelegate = new ComboBoxDelegate(this);
//    tableWidget->setItemDelegateForColumn(3,comboDelegate);


//    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
//    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);

    //tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //tableWidget->model()->setData(tableWidget->model()->index(row,column),Qt::AlignCenter,Qt::TextAlignmentRole);

//    QTableWidgetItem * protoitem = new QTableWidgetItem();
//    protoitem->setTextAlignment(Qt::AlignCenter);
//    protoitem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
//    tableWidget->setItemPrototype(protoitem);

//    shiftReset();
    //qDebug()<<"end shiftReset";

//    for(int j=0;j<3;++j) {
//        tableWidget->resizeColumnsToContents();
//        for(int i=0;i<9;++i) {
//            //qDebug()<<"for"<<i<<j;

//            tableWidget->item(i,j)->setTextAlignment( Qt::AlignCenter );
//            if(j==3)
//                tableWidget->item(i,j)->setFlags(tableWidget->item(i,j)->flags() & Qt::ItemIsEditable);
//            else
//                tableWidget->item(i,j)->setFlags(tableWidget->item(i,j)->flags() & ~Qt::ItemIsEditable);
//            //qDebug()<<"end if";

//        }
//    }
//    //qDebug()<<"end MainWindow";

}

MainWindow::~MainWindow()
{
    delete ui;
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

//void MainWindow::shiftReset()
//{
//    int itemsCount=9;
//    int startHour;
//    QTime ct = QTime::currentTime();
//    if(ct.hour()<6) {
//        itemsCount=6;
//        startHour=0;
//    }
//    else if (ct.hour()<15)
//        startHour=6;
//    else
//        startHour=15;

//    for(int i=0;i<9;++i) {
//        if(i<itemsCount){
//            QString period=QString("%1.00-%2.00").arg(startHour+i).arg((startHour+i+1)%24);
//            tableWidget->setItem(i, 0, new QTableWidgetItem(period));
//            tableWidget->setItem(i, 1, new QTableWidgetItem("0"));
//            tableWidget->setItem(i, 2, new QTableWidgetItem("0"));
//        } else {
//            tableWidget->setItem(i, 0, new QTableWidgetItem(""));
//            tableWidget->setItem(i, 1, new QTableWidgetItem(""));
//            tableWidget->setItem(i, 2, new QTableWidgetItem(""));
//        }
//        tableWidget->setItem(i, 3, new QTableWidgetItem(""));
//    }


//}

//void MainWindow::toggleRow(int row)
//{
//    tableWidget->setColumnHidden(3,!tableWidget->isColumnHidden(3));
//    tableWidget->setColumnHidden(4,!tableWidget->isColumnHidden(4));
//    tableWidget->setColumnHidden(6,!tableWidget->isColumnHidden(6));
//    for(int i=0;i<9;++i) {
//        tableWidget->setItem(i, 0, new QTableWidgetItem(""));
//        tableWidget->setItem(i, 1, new QTableWidgetItem(""));
//        tableWidget->setItem(i, 2, new QTableWidgetItem(""));
//    }


//}

