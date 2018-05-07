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
    qRegisterMetaType<QList<QList<hourItem>>>("QList<QList<hourItem>>");
    qRegisterMetaType<QList<planItem>>("QList<planItem>");
    Planner *planner = new Planner;
    //planner->readExcelData();
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



    return a.exec();
}


