#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QHeaderView>
#include "ComboBoxDelegate.h"

MainWindow::MainWindow(QWidget *parent) :
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
    QTableWidget *tableWidget;
    tableWidget = new QTableWidget(9, 4, this);

    QStringList headers;
    headers << "Период" <<  "План" <<  "Факт" <<  "Причина простоя";
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setVerticalHeaderLabels(QStringList()<<""<<""<<""<<""<<""<<""<<""<<""<<"");

    //QGridLayout * gridlay = new QGridLayout;
    //tableWidget->setLayout(gridlay);
    this->setCentralWidget(tableWidget);
    tableWidget->setColumnWidth(0,250);
    tableWidget->horizontalHeader()->setFont(QFont("Arial",30));
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
    //tableWidget->horizontalHeader()->setDefaultSectionSize(100);
    tableWidget->horizontalHeader()->setSectionsClickable(false);
    tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    tableWidget->setFont(QFont("Helvetica [Croyx]",30));
    ComboBoxDelegate *comboDelegate = new ComboBoxDelegate(this);
    tableWidget->setItemDelegateForColumn(3,comboDelegate);


    //tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    //tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //tableWidget->model()->setData(tableWidget->model()->index(row,column),Qt::AlignCenter,Qt::TextAlignmentRole);

//    QTableWidgetItem * protoitem = new QTableWidgetItem();
//    protoitem->setTextAlignment(Qt::AlignCenter);
//    protoitem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
//    tableWidget->setItemPrototype(protoitem);

    tableWidget->setItem(0, 0, new QTableWidgetItem("6.00-7.00"));
    tableWidget->setItem(1, 0, new QTableWidgetItem("7.00-8.00"));
    tableWidget->setItem(2, 0, new QTableWidgetItem("8.00-9.00"));
    tableWidget->setItem(3, 0, new QTableWidgetItem("9.00-10.00"));
    tableWidget->setItem(4, 0, new QTableWidgetItem("10.00-11.00"));
    tableWidget->setItem(5, 0, new QTableWidgetItem("11.00-12.00"));
    tableWidget->setItem(6, 0, new QTableWidgetItem("12.00-13.00"));
    tableWidget->setItem(7, 0, new QTableWidgetItem("13.00-14.00"));
    tableWidget->setItem(8, 0, new QTableWidgetItem("14.00-15.00"));

    tableWidget->setItem(0, 1, new QTableWidgetItem("100"));
    tableWidget->setItem(1, 1, new QTableWidgetItem("100"));
    tableWidget->setItem(2, 1, new QTableWidgetItem("100"));
    tableWidget->setItem(3, 1, new QTableWidgetItem("100"));
    tableWidget->setItem(4, 1, new QTableWidgetItem("50"));
    tableWidget->setItem(5, 1, new QTableWidgetItem("100"));
    tableWidget->setItem(6, 1, new QTableWidgetItem("100"));
    tableWidget->setItem(7, 1, new QTableWidgetItem("100"));
    tableWidget->setItem(8, 1, new QTableWidgetItem("100"));

    tableWidget->setItem(0, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(1, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(2, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(3, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(4, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(5, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(6, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(7, 2, new QTableWidgetItem("0"));
    tableWidget->setItem(8, 2, new QTableWidgetItem("0"));

    for(int j=0;j<3;++j) {
        tableWidget->resizeColumnsToContents();
        for(int i=0;i<9;++i) {
            tableWidget->item(i,j)->setTextAlignment( Qt::AlignCenter );
            if(j==3){
                tableWidget->item(i,j)->setFlags(tableWidget->item(i,j)->flags() & Qt::ItemIsEditable);

            }
            else
                tableWidget->item(i,j)->setFlags(tableWidget->item(i,j)->flags() & ~Qt::ItemIsEditable);
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
