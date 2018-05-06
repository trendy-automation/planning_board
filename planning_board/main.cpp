#include "mainwindow.h"
#include <QApplication>
#include "message_handler.h"
#include "single_apprun.h"
#include <QDebug>
#include <QTimer>
#include <QThread>
#include "planner.h"
#include <QDateTime>

#include <windows.h>
#include <iostream>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MessageHandler *msgHandler = new MessageHandler;
    qRegisterMetaType<QList<int>>("QList<int>");
//    qRegisterMetaType<QList<planItem>>("QList<planItem>");
    Planner *planner = new Planner;
    planner->readExcelData();
    //qtimer update screen
    //button update work content
    //memory of plan start time for recalc on addPlan

    QThread *keyThread = new QThread;
    QTimer *keyTimer = new QTimer(0);
    QByteArray *buffer = new QByteArray;
    QByteArray *buf = new QByteArray;

    QObject::connect(keyTimer,&QTimer::timeout,[buf,buffer,planner](){
        for (char i = 0; i < 255; i++)
        {
            int keyState = GetAsyncKeyState(i);
            if (keyState == 1 || keyState == -32767)
            {
                if((short)i==13){
                    buffer->clear();
                    buffer->append(*buf);
                    buf->clear();
                    planner->addPlan(*buffer);
                    qDebug()<<*buffer;
                }
                if((short)i>=48 && (short)i<=57)
                    buf->operator +=(i);
                else
                    buf->clear();
                //qDebug() << i << " " << (short)i;
                break;
            }
        }
    });
    keyTimer->moveToThread(keyThread);
    QObject::connect(keyThread, &QThread::started, keyTimer, [keyTimer](){keyTimer->start(10);});
    keyThread->start();
//    QTimer::singleShot(0,keyTimer,[](){keyTimer->start(100);});


    QStringList args = a.arguments();
    SingleAppRun singleApp(args.contains(APP_OPTION_FORCE),&a);
    if(singleApp.isToQuit()){
        a.quit();
        return 0;
    }


    MainWindow w(planner);
    w.show();
    //QObject::connect(planner,&Planner::planChanged,&w,&MainWindow::updatePlan);


//    QTimer *tableTimer = new QTimer(0);
//    QObject::connect(tableTimer,&QTimer::timeout,[tableTimer,planner,&w](){
//        QTime ct = QTime::currentTime();
//        //qDebug()<<"tableTimer lymbda"<<qMax(60000-2000,ct.msecsTo(QTime(ct.hour(),ct.minute(),59))+1000);
//        tableTimer->start(qMax(3600000-2000,ct.msecsTo(QTime(ct.hour(),59,59))+1000));
//        //tableTimer->start(qMax(60000-2000,ct.msecsTo(QTime(ct.hour(),ct.minute(),59))+1000));
//        planner->addPlan("");
//        if(ct.hour()==0||ct.hour()==6||ct.hour()==15){
//            w.shiftReset();
//            //excel shift report
//        }

//    });
//    QTime ct = QTime::currentTime();
//    tableTimer->start(ct.msecsTo(QTime(ct.hour(),59,59)));
//    //tableTimer->start(ct.msecsTo(QTime(ct.hour(),ct.minute(),59))+1000);

    return a.exec();
}


