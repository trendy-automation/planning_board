#include "mainwindow.h"
#include <QApplication>
//#include "excel_class.h"
#include "message_handler.h"
#include "single_apprun.h"
#include <QDebug>
#include <QTimer>
#include <QThread>



#include <windows.h>
#include <iostream>
//#include <stdio.h>
//using namespace std;
//HHOOK hkb;
//HINSTANCE hins;

//LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardProc(int nCode,WPARAM wParam, LPARAM lParam)
//    {
//        if (wParam == VK_F1)
//            {
//        qDebug() <<"Вы нажали F1";
//            }
//        qDebug() << nCode << wParam << lParam;


//    return CallNextHookEx(hkb, nCode, wParam, lParam );
//    }

//BOOL __declspec(dllexport)__stdcall installhook()
//    {
//    hkb=SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardProc, hins,0);
//    return TRUE;
//    }


//#ifndef HINSTANCE
//Q_DECLARE_HANDLE(HINSTANCE);
//#endif
//QT_BEGIN_NAMESPACE
//Q_CORE_EXPORT HINSTANCE qWinAppInst();

//LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//    qDebug() << nCode << wParam << lParam;
//    return CallNextHookEx(NULL, nCode, wParam, lParam);
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MessageHandler *msgHandler = new MessageHandler;

//    if (SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, qWinAppInst(), NULL) == 0)
//        qDebug() << "Hook failed for application instance" << qWinAppInst() << "with error:" << GetLastError();
//    hkb=SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)KeyboardProc, hins,0);
    QThread *keyThread = new QThread;
    QTimer *keyTimer = new QTimer(0);
    QByteArray *buffer = new QByteArray;
    QByteArray *buf = new QByteArray;

    QObject::connect(keyTimer,&QTimer::timeout,[buf,buffer](){
        for (char i = 0; i < 255; i++)
        {
            int keyState = GetAsyncKeyState(i);
            if (keyState == 1 || keyState == -32767)
            {
                if((short)i==13){
                    buffer->clear();
                    buffer->append(*buf);
                    buf->clear();
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


    MainWindow w;
    w.show();

    return a.exec();
}
