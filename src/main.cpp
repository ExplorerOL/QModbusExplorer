#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <QDir>
#include <QTranslator>

#include "QsLog.h"
#include "QsLogDest.h"
#include "mainwindow.h"
#include "modbusadapter.h"
#include "modbuscommsettings.h"

QTranslator *Translator;

int main(int argc, char *argv[])
{
    EUtils utils;

    //qt dpi warnings
    qputenv("QT_DEVICE_PIXEL_RATIO", "0");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    qputenv("QT_SCREEN_SCALE_FACTORS", "1");
    qputenv("QT_SCALE_FACTOR", "1");

    //application startup
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    Translator = new QTranslator;
    Translator->load(":/translations/" + QCoreApplication::applicationName() + "_" + QLocale::system().name());
    app.installTranslator(Translator);

    //init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::OffLevel); // start with no logging
    const QString sLogPath(QDir(app.applicationDirPath()).filePath("QModbusExplorer.log"));
    QsLogging::DestinationPtr fileDestination(QsLogging::DestinationFactory::MakeFileDestination(sLogPath,true,65535,7));
    QsLogging::DestinationPtr debugDestination(QsLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);



    //Modbus Adapter
    ModbusAdapter modbus_adapt(NULL);
    //Program settings
    ModbusCommSettings settings("QModbusExplorer.ini");

    //show main window
    mainWin = new MainWindow(NULL, &modbus_adapt, &settings);
    //connect signals - slots
    QObject::connect(&modbus_adapt, SIGNAL(refreshView()), mainWin, SLOT(refreshView()));
    QObject::connect(mainWin, SIGNAL(resetCounters()), &modbus_adapt, SLOT(resetCounters()));

    QsLogging::DestinationPtr objectDestination(QsLogging::DestinationFactory::MakeFunctorDestination(mainWin, SLOT(showLogData(QString,int))));
    logger.addDestination(objectDestination);

    mainWin->show();


    return app.exec();

}
