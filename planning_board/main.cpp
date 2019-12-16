#include "mainwindow.h"
#include <QApplication>
#include "message_handler.h"
#include "single_apprun.h"
//#include "watchdog.h"
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QSettings>
//#include <QHostAddress>
//#include <QNetworkInterface>
#include "plc_station.h"
#include "planner.h"


#include <windows.h>
#include <iostream>


//typedef QVector<TaskInfo> TaskInfoList;
//Q_DECLARE_METATYPE(TaskInfo)
Q_DECLARE_METATYPE(TaskInfoList)
//Q_DECLARE_METATYPE(QVector<TaskInfo>)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QStringList args = a.arguments();
//    Watchdog *watchdog = new Watchdog(&a);
//    if(args.contains(APP_OPTION_WATHCDOG)){
//        if(!watchdog->listen(JSONRPC_CLIENT_WATCHDOG_PORT,QString(JSONRPC_WATCHDOG_SERVICENAME).append(".isAlive")))
//            qDebug() << "Watchdog application cannot run!";
//        return a.exec();
//    }
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextEncoder *encoderWithoutBom = codec->makeEncoder( QTextCodec::IgnoreHeader );
////    QString str("Foobar")
////    QByteArray bytes  = encoderWithoutBom ->fromUnicode( s );

    MessageHandler *msgHandler = new MessageHandler;
    qDebug()<<"MessageHandler finished";
    qRegisterMetaTypeStreamOperators<TaskInfo>("TaskInfo");
    qRegisterMetaType<TaskInfoList>("TaskInfoList");
    //qRegisterMetaType<QVector<TaskInfo>>("TaskInfoList");
    qRegisterMetaType<TaskInfo>("TaskInfo>");
    qRegisterMetaType<QVector<TaskInfo>>("QVector<TaskInfo>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<kanbanItem>("kanbanItem");
    Planner *planner = new Planner();
    qDebug()<<"Planner finished";
    //planner->readExcelData();
    //button update work content
    //memory of plan start time for recalc on addPlan

    QThread *keyThread = new QThread;
    QTimer *keyTimer = new QTimer(0);
    QByteArray *buffer = new QByteArray;

    QObject::connect(keyTimer,&QTimer::timeout,[buffer,planner](){
        for (short i = 0; i < 255; i++)
        {
            int keyState = GetAsyncKeyState(i);
            if (keyState == 1 || keyState == -32767)
            {
                if(i==13){
                    //planner->addKanban(*buffer);
                    if (!buffer->isEmpty())
                        planner->parseBuffer(*buffer);
                    buffer->clear();
                }
                //if(i>=48 && i<=57)
                    buffer->operator +=(static_cast<char>(i));
                //else
                //    buffer->clear();
                //break;
            }
        }
    });

    keyTimer->moveToThread(keyThread);
    QObject::connect(keyThread, &QThread::started, keyTimer, [keyTimer](){keyTimer->start(1);});
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

    //########### Step 3 PLC_PARTNER connect ############
    //192.168.0.11
    //{"LocalAddress":"192.168.0.10", "LocTsap":"1002", "RemTsap":"2002",
    //"users":["RUTYABC018", "initial","RUTYABC019", "initial"]}
//    QByteArray LocalAddress("192.168.0.10");
//    QByteArray RemoteAddress("192.168.0.11");
    bool ok;
//    int LocTsap=QString("1002").toInt(&ok,16);
//    int RemTsap=QString("2002").toInt(&ok,16);

    QString FileName("settings.ini");
    QSettings settings(FileName, QSettings::IniFormat);

    QByteArray LocalAddress  = settings.value("LocalAddress","192.168.0.10").toByteArray();

//    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
//    QHostAddress ipAddress("192.168.0.11");
//    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
//        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
//             ipAddress=address;
//    }
//    QByteArray RemoteAddress = settings.value("RemoteAddress",ipAddress.toString()).toByteArray();

    QByteArray RemoteAddress = settings.value("RemoteAddress","192.168.0.11").toByteArray();

    int LocTsap = settings.value("LocTsap",QString("1002").toInt(&ok,16)).toInt();
    int RemTsap = settings.value("RemTsap",QString("2002").toInt(&ok,16)).toInt();
    int minRowHeight = settings.value("minRowHeight",MIN_ROW_HEIGHT).toInt();

    settings.setValue("LocalAddress", LocalAddress);
    settings.setValue("LocTsap", LocTsap);
    settings.setValue("RemoteAddress", RemoteAddress);
    settings.setValue("RemTsap", RemTsap);
    settings.setValue("minRowHeight", minRowHeight);



    qApp->setProperty("minRowHeight",minRowHeight);

    MainWindow w(planner);
    w.show();
    Plc_station * plcPartner = new Plc_station;
    QObject::connect(plcPartner, &Plc_station::dataReceived, planner, &Planner::kanbanProdused,Qt::QueuedConnection);
    plcPartner->setObjectName("DP_4B45X");
    plcPartner->setIdDevice(13);
    plcPartner->StartTo(LocalAddress,RemoteAddress,static_cast<word>(LocTsap),static_cast<word>(RemTsap));
    QStringList plcStatusList;
    plcStatusList<<"stopped"<<"running and active connecting"<<"running and waiting for a connection"
                 <<"running and connected : linked"<<"sending data"<<"receiving data"
                 <<"error starting passive server";
    qDebug()<<"plcPartner status:"<<plcPartner->getStatus()<<plcStatusList.at(plcPartner->getStatus());
    qDebug()<<"plcPartner finished";

    return a.exec();
}


