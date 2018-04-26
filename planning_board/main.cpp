#include "mainwindow.h"
#include <QApplication>
//#include "excel_class.h"
#include "message_handler.h"
#include "single_apprun.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MessageHandler msgHandler;
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
