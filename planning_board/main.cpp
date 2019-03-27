#include "mainwindow.h"
#include <QApplication>
#include "message_handler.h"
#include "single_apprun.h"
#include <QDebug>
#include <QTimer>
#include <QThread>
#include "planner.h"

#include <windows.h>
#include <iostream>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MessageHandler *msgHandler = new MessageHandler;
//    qRegisterMetaType<TaskInfoList>("TaskInfoList");
//    qRegisterMetaType<TaskInfo>("TaskInfo>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<kanbanItem>("kanbanItem");
    Planner *planner = new Planner();
    //planner->readExcelData();
    //button update work content
    //memory of plan start time for recalc on addPlan

    QThread *keyThread = new QThread;
    QTimer *keyTimer = new QTimer(0);
    QByteArray *buffer = new QByteArray;

    QObject::connect(keyTimer,&QTimer::timeout,[buffer,planner](){
        for (char i = 0; i < 255; i++)
        {
            int keyState = GetAsyncKeyState(i);
            if (keyState == 1 || keyState == -32767)
            {
                if((short)i==13){
                    //planner->addKanban(*buffer);
                    planner->parseBuffer(*buffer);
                    buffer->clear();
                }
                if((short)i>=48 && (short)i<=57)
                    buffer->operator +=(i);
                else
                    buffer->clear();
                break;
            }
        }
    });
    keyTimer->moveToThread(keyThread);
    QObject::connect(keyThread, &QThread::started, keyTimer, [keyTimer](){keyTimer->start(10);});
    keyThread->start();

    QStringList args = a.arguments();
    SingleAppRun singleApp(args.contains(APP_OPTION_FORCE),&a);
    if(singleApp.isToQuit()){
        a.quit();
        return 0;
    }

//    QFont font;
//    font.setFamily(font.defaultFamily());
//    a.setFont(font);

    MainWindow w(planner);
    w.show();

    return a.exec();
}


