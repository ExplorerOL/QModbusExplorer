#include <QtGui/QApplication>
#include <stdio.h>
#include <stdlib.h>
#include "mainwindow.h"

void stderrorMessageOutput(QtMsgType type, const char *msg)
{
     //in this function, you can write the message to any stream!
     switch (type) {
     case QtDebugMsg:
         fprintf(stderr, "Debug: %s\n", msg);
         break;
     case QtWarningMsg:
         fprintf(stderr, "Warning: %s\n", msg);
         break;
     case QtCriticalMsg:
         fprintf(stderr, "Critical: %s\n", msg);
         break;
     case QtFatalMsg:
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
     }
}

int main(int argc, char *argv[])
{
    //qInstallMsgHandler(stderrorMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
