#include <QtDebug>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QTranslator>
#include <QMessageBox>

#include "QsLog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "eutils.h"

MainWindow *mainWin;

MainWindow::MainWindow(QWidget *parent, ModbusAdapter *adapter, ModbusCommSettings *settings) :
    QMainWindow(parent), m_modbus(adapter), m_modbusCommSettings(settings),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //UI - dialogs
    m_dlgAbout = new About();
    connect(ui->actionAbout,SIGNAL(triggered()),m_dlgAbout,SLOT(show()));
    m_dlgModbusRTU = new SettingsModbusRTU(this,m_modbusCommSettings);
    connect(ui->actionSerial_RTU,SIGNAL(triggered()),this,SLOT(showSettingsModbusRTU()));
    m_dlgModbusTCP = new SettingsModbusTCP(this,m_modbusCommSettings);
    connect(ui->actionTCP,SIGNAL(triggered()),this,SLOT(showSettingsModbusTCP()));
    m_dlgSettings = new Settings(this,m_modbusCommSettings);
    connect(ui->actionSettings,SIGNAL(triggered()),this,SLOT(showSettings()));
    m_busMonitor = new BusMonitor(this, m_modbus->rawModel);
    connect(ui->actionBus_Monitor,SIGNAL(triggered()),this,SLOT(showBusMonitor()));

    //UI - connections
    connect(ui->cmbModbusMode,SIGNAL(currentIndexChanged(int)),this,SLOT(changedModbusMode(int)));
    connect(ui->cmbFunctionCode,SIGNAL(currentIndexChanged(int)),this,SLOT(changedFunctionCode(int)));
    connect(ui->cmbBase,SIGNAL(currentIndexChanged(int)),this,SLOT(changedBase(int)));
    connect(ui->cmbStartAddrBase,SIGNAL(currentIndexChanged(int)),this,SLOT(changedStartAddrBase(int)));
    connect(ui->sbSlaveID,SIGNAL(valueChanged(int)),this,SLOT(changedSlaveID(int)));
    connect(ui->sbNoOfRegs,SIGNAL(valueChanged(int)),this,SLOT(changedNoOfRegs(int)));
    connect(ui->sbStartAddress,SIGNAL(valueChanged(int)),this,SLOT(changedStartAddress(int)));
    connect(ui->spInterval,SIGNAL(valueChanged(int)),this,SLOT(changedScanRate(int)));
    connect(ui->actionClear,SIGNAL(triggered()),this,SLOT(clearItems()));
    connect(ui->actionRead_Write,SIGNAL(triggered()),this,SLOT(request()));
    connect(ui->actionScan,SIGNAL(toggled(bool)),this,SLOT(scan(bool)));
    connect(ui->actionConnect,SIGNAL(toggled(bool)),this,SLOT(changedConnect(bool)));
    connect(ui->actionReset_Counters,SIGNAL(triggered()),this,SIGNAL(resetCounters()));
    connect(ui->actionOpenLogFile,SIGNAL(triggered()),this,SLOT(openLogFile()));
    connect(ui->actionModbus_Manual,SIGNAL(triggered()),this,SLOT(openModbusManual()));
    connect(ui->actionEnglish_en_US,SIGNAL(triggered()),this,SLOT(changeLanguage()));
    connect(ui->actionSimplified_Chinese_zh_CN,SIGNAL(triggered()),this,SLOT(changeLanguage()));
    connect(ui->actionTraditional_Chinese_zh_TW,SIGNAL(triggered()),this,SLOT(changeLanguage()));
    connect(ui->actionLoad_Session,SIGNAL(triggered(bool)),this,SLOT(loadSession()));
    connect(ui->actionSave_Session,SIGNAL(triggered(bool)),this,SLOT(saveSession()));

    //UI - status
    m_statusInd = new QLabel;
    m_statusInd->setFixedSize( 16, 16 );
    m_statusText = new QLabel;
    m_baseAddr = new QLabel(tr("Base Addr : ") + "0");
    m_statusPackets = new QLabel(tr("Packets : ") + "0");
    m_statusPackets->setStyleSheet("QLabel {color:blue;}");
    m_statusErrors = new QLabel(tr("Errors : ") + "0");
    m_statusErrors->setStyleSheet("QLabel {color:red;}");
    ui->statusBar->addWidget(m_statusInd);
    ui->statusBar->addWidget(m_statusText, 10);
    ui->statusBar->addWidget(m_baseAddr, 10);
    ui->statusBar->addWidget(m_statusPackets, 10);
    ui->statusBar->addWidget(m_statusErrors, 10);
    m_statusInd->setPixmap(QPixmap(":/img/ballorange-16.png"));

    //Setup Toolbar
    ui->mainToolBar->addAction(ui->actionLoad_Session);
    ui->mainToolBar->addAction(ui->actionSave_Session);
    ui->mainToolBar->addAction(ui->actionConnect);
    ui->mainToolBar->addAction(ui->actionRead_Write);
    ui->mainToolBar->addAction(ui->actionScan);
    ui->mainToolBar->addAction(ui->actionClear);
    ui->mainToolBar->addAction(ui->actionReset_Counters);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionOpenLogFile);
    ui->mainToolBar->addAction(ui->actionBus_Monitor);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionSerial_RTU);
    ui->mainToolBar->addAction(ui->actionTCP);
    ui->mainToolBar->addAction(ui->actionSettings);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionModbus_Manual);
    ui->mainToolBar->addAction(ui->actionAbout);
    ui->mainToolBar->addAction(ui->actionExit);

    //Init models
    ui->tblRegisters->setItemDelegate(m_modbus->regModel->itemDelegate());
    ui->tblRegisters->setModel(m_modbus->regModel->model);
    m_modbus->regModel->setBase(EUtils::UInt);
    m_modbus->regModel->setStartAddrBase(10);

    //Update UI
    ui->sbNoOfRegs->setEnabled(true);
    ui->actionRead_Write->setEnabled(false);
    ui->actionScan->setEnabled(false);
    ui->sbStartAddress->setMinimum(m_modbusCommSettings->baseAddr().toInt());
    ui->cmbBase->setCurrentIndex(m_modbusCommSettings->base());
    ui->cmbFunctionCode->setCurrentIndex(m_modbusCommSettings->functionCode());
    ui->cmbModbusMode->setCurrentIndex(m_modbusCommSettings->modbusMode());
    ui->sbSlaveID->setValue(m_modbusCommSettings->slaveID());
    ui->spInterval->setValue(m_modbusCommSettings->scanRate());
    ui->sbStartAddress->setValue(m_modbusCommSettings->startAddr());
    ui->sbNoOfRegs->setValue(m_modbusCommSettings->noOfRegs());
    updateStatusBar();
    refreshView();

    //Logging level
    QsLogging::Logger::instance().setLoggingLevel((QsLogging::Level)m_modbusCommSettings->loggingLevel());
    QLOG_INFO()<<  "Start Program" ;

}

MainWindow::~MainWindow()
{

    if (m_modbus)
        m_modbus->modbusDisConnect();
    delete ui;

    QLOG_INFO()<<  "Stop Program" ;

}

void MainWindow::showSettingsModbusRTU()
{

    //Show RTU Settings Dialog

    if (m_dlgModbusRTU->exec()==QDialog::Accepted) {
        QLOG_TRACE()<<  "RTU settings changes accepted ";
        updateStatusBar();
        m_modbusCommSettings->saveSettings();
    }
    else
        QLOG_WARN()<<  "RTU settings changes rejected ";

}

void MainWindow::showSettingsModbusTCP()
{

    //Show TCP Settings Dialog

    if (m_dlgModbusTCP->exec()==QDialog::Accepted) {
        QLOG_TRACE()<<  "TCP settings changes accepted ";
        updateStatusBar();
        m_modbusCommSettings->saveSettings();
    }
    else
        QLOG_WARN()<<  "TCP settings changes rejected ";

}

void MainWindow::showSettings()
{

    //Show General Settings Dialog

    if (m_dlgSettings->exec()==QDialog::Accepted) {
        QLOG_TRACE()<<  "Settings changes accepted ";
        m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->maxNoOfLines().toInt());
        m_modbus->setTimeOut(m_modbusCommSettings->timeOut().toInt());
        m_modbusCommSettings->saveSettings();
    }
    else
        QLOG_WARN()<<  "Settings changes rejected ";

    updateStatusBar();

}

void MainWindow::showBusMonitor()
{

    //Show Bus Monitor

    m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->maxNoOfLines().toInt());
    m_busMonitor->move(this->x() + this->width() + 20, this->y());
    m_busMonitor->show();

}

void MainWindow::changedModbusMode(int currIndex)
{

    //Change lblSlave text : Slave Addr, Unit ID

    QLOG_TRACE()<<  "Modbus Mode changed. Index = " << currIndex;
    m_modbusCommSettings->setModbusMode(currIndex);
    m_modbusCommSettings->saveSettings();

    if (currIndex == 0) { //RTU
        ui->lblSlave->setText("Slave Addr");
    }
    else { //TCP
       ui->lblSlave->setText("Unit ID");
    }

    updateStatusBar();

}

void MainWindow::changedFunctionCode(int currIndex)
{

    //Enable-Disable number of coils or registers

    QLOG_TRACE()<<  "Function Code changed. Index = " << currIndex;
    m_modbusCommSettings->setFunctionCode(currIndex);
    m_modbusCommSettings->saveSettings();

    const int functionCode = EUtils::ModbusFunctionCode(currIndex);
    QString String_number_of_coils(tr("Number of Coils"));
    QString String_number_of_inputs(tr("Number of Inputs"));
    QString String_number_of_registers(tr("Number of Registers"));
    switch(functionCode)//Label = Read Request, Write Request
    {
        case MODBUS_FC_READ_COILS:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfRegs->setEnabled(true);
                ui->lblNoOfCoils->setText(String_number_of_coils);
                break;
        case MODBUS_FC_READ_DISCRETE_INPUTS:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfRegs->setEnabled(true);
                ui->lblNoOfCoils->setText(String_number_of_inputs);
                break;
        case MODBUS_FC_READ_HOLDING_REGISTERS:
                m_modbus->regModel->setIs16Bit(true);
                ui->sbNoOfRegs->setEnabled(true);
                ui->lblNoOfCoils->setText(String_number_of_registers);
                break;
        case MODBUS_FC_READ_INPUT_REGISTERS:
                m_modbus->regModel->setIs16Bit(true);
                ui->sbNoOfRegs->setEnabled(true);
                ui->lblNoOfCoils->setText(String_number_of_registers);
                break;
        case MODBUS_FC_WRITE_SINGLE_COIL:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfRegs->setValue(1);
                ui->sbNoOfRegs->setEnabled(false);
                ui->lblNoOfCoils->setText(String_number_of_coils);
                break;
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
                m_modbus->regModel->setIs16Bit(false);
                if (ui->sbNoOfRegs->value() < 2)
                    ui->sbNoOfRegs->setValue(2);
                ui->sbNoOfRegs->setEnabled(true);
                ui->lblNoOfCoils->setText(String_number_of_coils);
                break;
        case MODBUS_FC_WRITE_SINGLE_REGISTER:
                m_modbus->regModel->setIs16Bit(true);
                ui->sbNoOfRegs->setValue(1);
                ui->sbNoOfRegs->setEnabled(false);
                ui->lblNoOfCoils->setText(String_number_of_registers);
                break;
        case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
                m_modbus->regModel->setIs16Bit(true);
                if (ui->sbNoOfRegs->value() < 2)
                    ui->sbNoOfRegs->setValue(2);
                ui->sbNoOfRegs->setEnabled(true);
                ui->lblNoOfCoils->setText(String_number_of_registers);
                break;
        default:
                m_modbus->regModel->setIs16Bit(false);
                ui->sbNoOfRegs->setValue(1);
                ui->sbNoOfRegs->setEnabled(true);
                ui->lblNoOfCoils->setText(String_number_of_coils);
                break;
     }

    m_modbus->setNumOfRegs(ui->sbNoOfRegs->value());
    addItems();

}

void MainWindow::changedBase(int currIndex)
{

    //Change Base

    QLOG_TRACE()<<  "Base changed. Index = " << currIndex;
    m_modbusCommSettings->setBase(currIndex);
    m_modbusCommSettings->saveSettings();

    switch(currIndex)
    {
        case 0:
                m_modbus->regModel->setBase(EUtils::Bin);
                break;
        case 1:
                m_modbus->regModel->setBase(EUtils::UInt);
                break;
        case 2:
                m_modbus->regModel->setBase(EUtils::Hex);
                break;
        default:
                m_modbus->regModel->setBase(EUtils::UInt);
                break;
     }

}

void MainWindow::changedScanRate(int value)
{

    //Enable-Disable Time Interval

    QLOG_TRACE()<<  "ScanRate changed. Value = " << value;
    m_modbusCommSettings->setScanRate(value);
    m_modbusCommSettings->saveSettings();

    m_modbus->setScanRate(value);

}

void MainWindow::changedConnect(bool value)
{

    //Connect - Disconnect

    if (value) { //Connected
        modbusConnect(true);
        QLOG_INFO()<<  "Connected ";
    }
    else { //Disconnected
        modbusConnect(false);
        QLOG_INFO()<<  "Disconnected ";
    }

    m_modbus->resetCounters();
    refreshView();

}

void MainWindow::changedSlaveID(int value)
{

    //Slave ID

    QLOG_TRACE()<<  "Slave ID Changed. Value = " << value;
    m_modbusCommSettings->setSlaveID(value);
    m_modbusCommSettings->saveSettings();

}

void MainWindow::openLogFile()
{

    //Open log file
    QString arg;
    QLOG_TRACE()<<  "Open log file";

    arg = "file:///" + QCoreApplication::applicationDirPath() + "/QModMaster.log";
    QDesktopServices::openUrl(QUrl(arg));


}

void MainWindow::openModbusManual()
{

    //Open Modbus Manual
    QString arg;
    QLOG_TRACE()<<  "Open Modbus Manual";

    arg = "file:///" + QCoreApplication::applicationDirPath() + "/ManModbus/index.html";
    QDesktopServices::openUrl(QUrl(arg));


}

void MainWindow::changedStartAddrBase(int currIndex)
{

    //Change Base

    QLOG_TRACE()<<  "Start Addr Base changed. Index = " << currIndex;

    switch(currIndex)
    {
        case 0:
                ui->sbStartAddress->setDisplayIntegerBase(10);
                m_modbus->regModel->setStartAddrBase(10);
                break;
        case 1:
                 ui->sbStartAddress->setDisplayIntegerBase(16);
                 m_modbus->regModel->setStartAddrBase(16);
                break;
        default:
                ui->sbStartAddress->setDisplayIntegerBase(10);
                m_modbus->regModel->setStartAddrBase(10);
                break;
     }

}

void MainWindow::changedStartAddress(int value)
{

    //Start Address changed
    QLOG_TRACE()<<  "Start Address changed. Value = " << value;
    m_modbusCommSettings->setStartAddr(value);
    m_modbusCommSettings->saveSettings();

    m_modbus->setStartAddr(value);
    addItems();

}

void MainWindow::changedNoOfRegs(int value)
{

    //No of regs changed
    QLOG_TRACE()<<  "No Of Regs changed. Value = " << value;
    m_modbusCommSettings->setNoOfRegs(value);
    m_modbusCommSettings->saveSettings();

    m_modbus->setNumOfRegs(value);
    addItems();

}

void MainWindow::updateStatusBar()
{

    //Update status bar

    QString msg;

    if(ui->cmbModbusMode->currentIndex() == 0) { //RTU
        msg = "RTU : ";
        msg += m_modbusCommSettings->serialPortName() + " | ";
        msg += m_modbusCommSettings->baud() + ",";
        msg += m_modbusCommSettings->dataBits() + ",";
        msg += m_modbusCommSettings->stopBits() + ",";
        msg += m_modbusCommSettings->parity();
    }
    else {
        msg = "TCP : ";
        msg += m_modbusCommSettings->slaveIP() + ":";
        msg += m_modbusCommSettings->TCPPort();
    }

    m_statusText->clear();
    m_statusText->setText(msg);

    //Connection is valid
    if (m_modbus->isConnected()) {
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-green-16.png"));
    }
    else {
        m_statusInd->setPixmap(QPixmap(":/icons/bullet-red-16.png"));
    }

    //basr Address
    m_baseAddr->setText("Base Addr : " + m_modbusCommSettings->baseAddr());

}

void MainWindow::addItems()
{

    //add items

    m_modbus->setSlave(ui->sbSlaveID->value());
    m_modbus->setFunctionCode(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
    m_modbus->setStartAddr(ui->sbStartAddress->value());
    m_modbus->setNumOfRegs(ui->sbNoOfRegs->value());

    QLOG_INFO()<<  "Add Items. Function Code = " << QString::number(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()),16);

    m_modbus->addItems();

}

void MainWindow::clearItems()
{

    //Clear items from registers model

    QLOG_TRACE()<<  "clearItems" ;

    m_modbus->regModel->clear();
    addItems();

}

void MainWindow::request()
{

     //Request items from modbus adapter and add raw data to raw data model
    int rowCount = m_modbus->regModel->model->rowCount();
    int baseAddr;

    QLOG_TRACE()<<  "Request transaction. No or registers = " <<  rowCount;

    if (rowCount == 0) {
        mainWin->showUpInfoBar(tr("Request failed\nAdd items to Registers Table."), InfoBar::Error);
        QLOG_WARN()<<  "Request failed. No items in registers table ";
        return;
    }
    else {
        mainWin->hideInfoBar();
    }

    //get base address
    baseAddr = m_modbusCommSettings->baseAddr().toInt();

    m_modbus->setSlave(ui->sbSlaveID->value());
    m_modbus->setFunctionCode(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
    m_modbus->setStartAddr(ui->sbStartAddress->value() + baseAddr);
    m_modbus->setNumOfRegs(ui->sbNoOfRegs->value());

    //Modbus data
    m_modbus->modbusTransaction();

}

void MainWindow::scan(bool value)
{

   //Request items from modbus adapter and add raw data to raw data model
   int rowCount = m_modbus->regModel->model->rowCount();
   int baseAddr;

   if (value && rowCount == 0) {
       mainWin->showUpInfoBar(tr("Request failed\nAdd items to Registers Table."), InfoBar::Error);
       QLOG_WARN()<<  "Request failed. No items in registers table ";
       ui->actionScan->setChecked(false);
       return;
   }
   else {
       mainWin->hideInfoBar();
   }

   //get base address
   baseAddr = m_modbusCommSettings->baseAddr().toInt();

   m_modbus->setSlave(ui->sbSlaveID->value());
   m_modbus->setFunctionCode(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
   m_modbus->setStartAddr(ui->sbStartAddress->value() + baseAddr);
   m_modbus->setNumOfRegs(ui->sbNoOfRegs->value());

    //Start-Stop poll timer
    QLOG_TRACE()<<  "Scan time = " << value;
    if (value){
        if (ui->spInterval->value() < m_modbusCommSettings->timeOut().toInt() * 1000 * 2){
            mainWin->showUpInfoBar(tr("Scan rate  should be at least 2 * Timeout."), InfoBar::Error);
            QLOG_ERROR()<<  "Scan rate error. should be at least 2 * Timeout ";
        }
        else {
            m_modbus->setScanRate(ui->spInterval->value());
            m_modbus->startPollTimer();
        }
    }
    else
        m_modbus->stopPollTimer();

    ui->cmbBase->setEnabled(!value);
    ui->cmbFunctionCode->setEnabled(!value);
    ui->sbSlaveID->setEnabled(!value);
    ui->sbStartAddress->setEnabled(!value);
    ui->spInterval->setEnabled(!value);
    if (!value)
        changedFunctionCode(ui->cmbFunctionCode->currentIndex());
    else
        ui->sbNoOfRegs->setEnabled(false);

}

void MainWindow::modbusConnect(bool connect)
 {

    //Modbus connect - RTU/TCP
    QLOG_TRACE()<<  "Modbus Connect. Value = " << connect;

    if (connect) { //RTU
        if (ui->cmbModbusMode->currentIndex() == EUtils::RTU) {
            m_modbus->setSlave(ui->sbSlaveID->value());
            m_modbus->modbusConnectRTU(m_modbusCommSettings->serialPortName(),
                                        m_modbusCommSettings->baud().toInt(),
                                        EUtils::parity(m_modbusCommSettings->parity()),
                                        m_modbusCommSettings->dataBits().toInt(),
                                        m_modbusCommSettings->stopBits().toInt(),
                                        m_modbusCommSettings->RTS().toInt(),
                                        m_modbusCommSettings->timeOut().toInt()
                                        );
        }
        else { //TCP
            m_modbus->modbusConnectTCP(m_modbusCommSettings->slaveIP(),
                                       m_modbusCommSettings->TCPPort().toInt(),
                                       m_modbusCommSettings->timeOut().toInt());
        }
    }
    else { //Disconnect
        m_modbus->modbusDisConnect();
        ui->actionScan->setChecked(false);
    }

    updateStatusBar();

    //Update UI
    ui->actionConnect->setChecked(m_modbus->isConnected());
    ui->actionRead_Write->setEnabled(m_modbus->isConnected());
    ui->actionScan->setEnabled(m_modbus->isConnected());
    ui->cmbModbusMode->setEnabled(!m_modbus->isConnected());

 }

 void MainWindow::refreshView()
 {

     QLOG_TRACE()<<  "Packets sent / received = " << m_modbus->packets() << ", errors = " << m_modbus->errors();
     ui->tblRegisters->resizeColumnsToContents();

     m_statusPackets->setText(tr("Packets : ") + QString("%1").arg(m_modbus->packets()));
     m_statusErrors->setText(tr("Errors : ") + QString("%1").arg(m_modbus->errors()));

 }

void MainWindow::loadSession()
{

     QLOG_TRACE()<<  "load session";
    //debug
     QMessageBox msgBox;
     msgBox.setText("Load Session.");
     msgBox.exec();

}

void MainWindow::saveSession()
{

     QLOG_TRACE()<<  "save session";
    //debug
     QMessageBox msgBox;
     msgBox.setText("Save Session.");
     msgBox.exec();

}
void MainWindow::showUpInfoBar(QString message, InfoBar::InfoType type)
{
    ui->infobar->show(message, type);
}

void MainWindow::hideInfoBar()
{
    ui->infobar->hide();
}

void MainWindow::changeEvent(QEvent* event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::changeLanguage()
{
    extern QTranslator *Translator;
    QCoreApplication::removeTranslator(Translator);
    Translator->load(":/translations/" + QCoreApplication::applicationName() + sender()->objectName().right(6));
    QCoreApplication::installTranslator(Translator);
}

