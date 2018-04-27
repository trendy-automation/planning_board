#include "mainwindow.h"
#include <QApplication>
//#include "excel_class.h"
#include "message_handler.h"
#include "single_apprun.h"
#include <QDebug>
#include <windows.h>

#ifndef HINSTANCE
Q_DECLARE_HANDLE(HINSTANCE);
#endif
QT_BEGIN_NAMESPACE
Q_CORE_EXPORT HINSTANCE qWinAppInst();

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    qDebug() << nCode << wParam << lParam;
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MessageHandler *msgHandler = new MessageHandler;

    if (SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, qWinAppInst(), NULL) == 0)
        qDebug() << "Hook failed for application instance" << qWinAppInst() << "with error:" << GetLastError();

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
