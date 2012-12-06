#include <QTextCodec>
#include <QtDebug>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "eutils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Text codec
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //Settings File
    m_modbusCommSettings = new QSettings("qModMaster.ini",QSettings::IniFormat);
    //Modbus Adapter
    m_modbus=new ModbusAdapter(this);
    m_modbus->regModel->setBase(EUtils::Bin);
    //Registers Table Delegate
    ui->tblRegisters->setItemDelegate(new RegistersDataDelegate(this));
    m_regDataDelegate = static_cast<RegistersDataDelegate*>(ui->tblRegisters->itemDelegate());

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
    connect(ui->sbNoOfCoils,SIGNAL(valueChanged(int)),this,SLOT(clearItems()));
    connect(ui->sbStartAddress,SIGNAL(valueChanged(int)),this,SLOT(clearItems()));
    connect(ui->chkReqCycle,SIGNAL(clicked(bool)),this,SLOT(changedReqCycle(bool)));
    connect(ui->btAddItems,SIGNAL(clicked(bool)),this,SLOT(addItems()));
    connect(ui->btClear,SIGNAL(clicked(bool)),this,SLOT(clearItems()));
    connect(ui->btRequest,SIGNAL(clicked(bool)),this,SLOT(request()));
    connect(ui->btConnect,SIGNAL(toggled(bool)),this,SLOT(changedConnect(bool)));
    //UI - status
    m_statusInd = new QWidget;
    m_statusInd->setFixedSize( 16, 16 );
    m_statusText = new QLabel;
    ui->statusBar->addWidget(m_statusInd);
    ui->statusBar->addWidget(m_statusText, 10);
    m_statusInd->setStyleSheet("background-color: red");
    //Populate function name combo box
    ui->cmbFunctionCode->clear();
    for(int i=0; i<8; i++)
        ui->cmbFunctionCode->addItem(EUtils::ModbusFunctionName(i));

    //Setup Toolbar
    ui->mainToolBar->addAction(ui->actionBus_Monitor);
    ui->mainToolBar->addAction(ui->actionSerial_RTU);
    ui->mainToolBar->addAction(ui->actionTCP);
    ui->mainToolBar->addAction(ui->actionSettings);
    ui->mainToolBar->addAction(ui->actionAbout);
    ui->mainToolBar->addAction(ui->actionExit);

    //Init Code
    ui->tblRegisters->setModel(m_modbus->regModel->model);
    m_modbus->regModel->setBase(EUtils::Bin);
    connect(m_modbus,SIGNAL(refreshView()),this,SLOT(refreshView()));
    connect(m_modbus->regModel,SIGNAL(refreshView()),this,SLOT(refreshView()));
    //init settings
    //init MaxNoOfLines to 50 if is not defined yet
    m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->value("MaxNoOfLines").toInt() == 0 ? 50 : m_modbusCommSettings->value("MaxNoOfLines").toInt());
    m_timeOut = m_modbusCommSettings->value("TimeOut").toInt() == 0 ? 1 : m_modbusCommSettings->value("TimeOut").toInt();
    //init TCP settings if not defined yet
    m_modbusCommSettings->setValue("TCPPort",(m_modbusCommSettings->value("TCPPort").isNull() ? "502" : m_modbusCommSettings->value("TCPPort").toString()));
    m_modbusCommSettings->setValue("SlaveIPByte1",(m_modbusCommSettings->value("SlaveIPByte1").toInt() == 0 ? "127" : m_modbusCommSettings->value("SlaveIPByte1").toString()));
    m_modbusCommSettings->setValue("SlaveIPByte2",(m_modbusCommSettings->value("SlaveIPByte2").toInt() == 0 ? "0" : m_modbusCommSettings->value("SlaveIPByte2").toString()));
    m_modbusCommSettings->setValue("SlaveIPByte3",(m_modbusCommSettings->value("SlaveIPByte3").toInt() == 0 ? "0" : m_modbusCommSettings->value("SlaveIPByte3").toString()));
    m_modbusCommSettings->setValue("SlaveIPByte4",(m_modbusCommSettings->value("SlaveIPByte4").toInt() == 0 ? "1" : m_modbusCommSettings->value("SlaveIPByte4").toString()));
    m_modbusCommSettings->setValue("SlaveIP",(m_modbusCommSettings->value("SlaveIPByte1").toString() + "." + m_modbusCommSettings->value("SlaveIPByte2").toString() + "." +
                                    m_modbusCommSettings->value("SlaveIPByte3").toString() + "." + m_modbusCommSettings->value("SlaveIPByte4").toString()));
    //init RTU settings if not defined yet
    m_modbusCommSettings->setValue("SerialPort",(m_modbusCommSettings->value("SerialPort").isNull() ? "COM1" : m_modbusCommSettings->value("SerialPort").toString()));
    m_modbusCommSettings->setValue("SerialPortOS",(m_modbusCommSettings->value("SerialPortOS").isNull() ? "COM1" : m_modbusCommSettings->value("SerialPortOS").toString()));
    m_modbusCommSettings->setValue("Baud",(m_modbusCommSettings->value("Baud").isNull() ? "9600" : m_modbusCommSettings->value("Baud").toString()));
    m_modbusCommSettings->setValue("DataBits",(m_modbusCommSettings->value("DataBits").isNull() ? "8" : m_modbusCommSettings->value("DataBits").toString()));
    m_modbusCommSettings->setValue("StopBits",(m_modbusCommSettings->value("StopBits").isNull() ? "1" : m_modbusCommSettings->value("StopBits").toString()));
    m_modbusCommSettings->setValue("Parity",(m_modbusCommSettings->value("Parity").isNull() ? "None" : m_modbusCommSettings->value("Parity").toString()));
#ifdef Q_OS_WIN32
    m_modbusCommSettings->setValue("RTS",(m_modbusCommSettings->value("RTS").isNull() ? "Disable" : m_modbusCommSettings->value("RTS").toString()));
#else
    m_modbusCommSettings->setValue("RTS",(m_modbusCommSettings->value("RTS").isNull() ? "None" : m_modbusCommSettings->value("RTS").toString()));
#endif

    //Poll Timer
    m_pollTimer=new QTimer(this);
    connect(m_pollTimer,SIGNAL(timeout()),this,SLOT(pollRequestForData()));

     //Update UI
    ui->sbNoOfCoils->setEnabled(true);
    ui->spInterval->setEnabled(false);
    ui->btRequest->setEnabled(false);
    ui->btAddItems->setEnabled(false);
    updateStatusBar();

    qDebug()<<  "MainWindow : Init Completed ";

}

MainWindow::~MainWindow()
{
    delete ui;

    qDebug()<<  "MainWindow : Closing" ;
}

void MainWindow::showSettingsModbusRTU()
{

    //Show RTU Settings Dialog

    if (m_dlgModbusRTU->exec()==QDialog::Accepted) {
        qDebug()<<  "MainWindow : RTU changes accepted ";
        updateStatusBar();
    }
    else
        qDebug()<<  "MainWindow : RTU changes rejected ";

}

void MainWindow::showSettingsModbusTCP()
{

    //Show TCP Settings Dialog

    if (m_dlgModbusTCP->exec()==QDialog::Accepted) {
        qDebug()<<  "MainWindow : TCP changes accepted ";
        updateStatusBar();
    }
    else
        qDebug()<<  "MainWindow : TCP changes rejected ";

}

void MainWindow::showSettings()
{

    //Show General Settings Dialog

    if (m_dlgSettings->exec()==QDialog::Accepted) {
        qDebug()<<  "MainWindow : changes accepted ";
        m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->value("MaxNoOfLines").toInt());
        m_timeOut = m_modbusCommSettings->value("TimeOut").toInt();
    }
    else
        qDebug()<<  "MainWindow : changes rejected ";

}

void MainWindow::showBusMonitor()
{

    //Show Bus Monitor

    qDebug()<<  "MainWindow : showBusMonitor ";

    m_busMonitor->show();

}

void MainWindow::changedModbusMode(int currIndex)
{

    //Show-Hide Slave IP

    qDebug()<<  "MainWindow : changedModbusMode index = " << currIndex;

    if (currIndex == 0) { //RTU

    }
    else { //TCP

    }

    updateStatusBar();

}

void MainWindow::changedFunctionCode(int currIndex)
{

    //Enable-Disable number of coils

    qDebug()<<  "MainWindow : changedFunctionCode index = " << currIndex;

    const int funcionCode = EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex());

    switch(funcionCode)//Number of coils
    {
        case _FC_WRITE_SINGLE_REGISTER:
        case _FC_WRITE_SINGLE_COIL:
                ui->sbNoOfCoils->setValue(1);
                ui->sbNoOfCoils->setEnabled(false);
                break;
        case _FC_WRITE_MULTIPLE_COILS:
        case _FC_WRITE_MULTIPLE_REGISTERS:
                ui->sbNoOfCoils->setValue(2);
                ui->sbNoOfCoils->setEnabled(true);
                break;
        default:
                ui->sbNoOfCoils->setEnabled(true);
                break;
     }

    switch(funcionCode)//Base = Binary
    {
        case _FC_READ_COILS:
        case _FC_READ_DISCRETE_INPUTS:
        case _FC_WRITE_SINGLE_COIL:
        case _FC_WRITE_MULTIPLE_COILS:
                ui->cmbBase->setCurrentIndex(0);
                ui->cmbBase->setEnabled(false);
                break;
        default:
                ui->cmbBase->setEnabled(true);
                break;
     }

    switch(funcionCode)//Label = Read Request, Write Request
    {
        case _FC_READ_COILS:
        case _FC_READ_DISCRETE_INPUTS:
        case _FC_READ_HOLDING_REGISTERS:
        case _FC_READ_INPUT_REGISTERS:
                ui->btRequest->setText("Read Request");
                break;
        case _FC_WRITE_SINGLE_COIL:
        case _FC_WRITE_MULTIPLE_COILS:
        case _FC_WRITE_SINGLE_REGISTER:
        case _FC_WRITE_MULTIPLE_REGISTERS:
                ui->btRequest->setText("Write Request");
                break;
        default:
                ui->btRequest->setText("Read Request");
                break;
     }

    switch(funcionCode)//Set is16Bit
    {
        case _FC_READ_HOLDING_REGISTERS:
        case _FC_READ_INPUT_REGISTERS:
        case _FC_WRITE_SINGLE_REGISTER:
        case _FC_WRITE_MULTIPLE_REGISTERS:
                m_regDataDelegate->setIs16Bit(true);
                m_modbus->regModel->setIs16Bit(true);
                break;
        default:
                m_regDataDelegate->setIs16Bit(false);
                m_modbus->regModel->setIs16Bit(false);
                break;
     }

    clearItems();

}

void MainWindow::changedBase(int currIndex)
{

    //Change Base

    qDebug()<<  "MainWindow : changedBase index = " << currIndex;

    switch(currIndex)
    {
        case 0:
                m_modbus->regModel->setBase(EUtils::Bin);
                m_regDataDelegate->setBase(EUtils::Bin);
                break;
        case 1:
                m_modbus->regModel->setBase(EUtils::UInt);
                m_regDataDelegate->setBase(EUtils::UInt);
                break;
        /* --- TODO ---
        case 2:
                m_modbus->regModel->setBase(EUtils::SInt);
                m_regDataDelegate->setBase(EUtils::SInt);
                break;
        */
        case 2:
                m_modbus->regModel->setBase(EUtils::Hex);
                m_regDataDelegate->setBase(EUtils::Hex);
                break;
        default:
                m_modbus->regModel->setBase(EUtils::UInt);
                m_regDataDelegate->setBase(EUtils::UInt);
                break;
     }

}

void MainWindow::changedReqCycle(bool value)
{

    //Enable-Disable Time Interval - Set-Reset btRequest As Toggle button

    qDebug()<<  "MainWindow : changedReqCycle value = " << value;

    ui->spInterval->setEnabled(value);
    ui->btRequest->setCheckable(value);
    ui->btRequest->repaint();

    //stop poll timer
    if (!value) m_pollTimer->stop();

}

void MainWindow::changedConnect(bool value)
{

    //Connect - Disconnect

    qDebug()<<  "MainWindow : changedConnect value = " << value;

    if (value) { //Connected
        ui->btConnect->setText("Disconnect");
        ui->btConnect->setToolTip("Disconnect");
        modbusConnect(true);
    }
    else { //Disconnected
        ui->btConnect->setText("Connect");
        ui->btConnect->setToolTip("Connect");
        clearItems();
        modbusConnect(false);
    }

}

void MainWindow::changedSlaveIP()
{


}

void MainWindow::updateStatusBar()
{

    //Update status bar

    QString msg;

    if(ui->cmbModbusMode->currentIndex() == 0) { //RTU
        msg = "RTU : ";
        msg += m_modbusCommSettings->value("SerialPort").toString() + " | ";
        msg += m_modbusCommSettings->value("Baud").toString() + ",";
        msg += m_modbusCommSettings->value("DataBits").toString() + ",";
        msg += m_modbusCommSettings->value("StopBits").toString() + ",";
        msg += m_modbusCommSettings->value("Parity").toString();
    }
    else {
        msg = "TCP : ";
        msg += m_modbusCommSettings->value("SlaveIP").toString() + ":";
        msg += m_modbusCommSettings->value("TCPPort").toString();
    }

    m_statusText->clear();
    m_statusText->setText(msg);

    //Connection is valid
    if (m_modbus->isConnected()) {
        m_statusInd->setStyleSheet("background-color: green");
        qDebug()<<  "MainWindow : updateStatusBar - ind color = green" ;
    }
    else {
        m_statusInd->setStyleSheet("background-color: red");
        qDebug()<<  "MainWindow : updateStatusBar - ind color = red" ;
    }

}

void MainWindow::addItems()
{

    //Add items to registers model and to modbus adapter only if we are connected
    if (!m_modbus->isConnected())
        return;

    const int slave = ui->sbSlaveID->value();
    const int functionCode = EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex());
    const int addr = ui->sbStartAddress->value();
    const int num = ui->sbNoOfCoils->value();
    const QString dataType = EUtils::ModbusDataTypeName(functionCode);
    bool valueIsEditable = EUtils::ModbusIsWriteFunction(functionCode);

    qDebug()<<  "MainWindow : addItems - functionCode " << QString::number(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()),16);

    ui->lblRegisters->setText("Registers > " + dataType);
    m_modbus->regModel->addItems(ui->sbStartAddress->text().toInt(),ui->sbNoOfCoils->text().toInt(),valueIsEditable);
    //If it is a write function -> read registers
    if (EUtils::ModbusIsWriteCoilsFunction(functionCode))
        m_modbus->modbusTransaction(slave,EUtils::ReadCoils,addr,num);
    else if (EUtils::ModbusIsWriteRegistersFunction(functionCode))
        m_modbus->modbusTransaction(slave,EUtils::ReadHoldRegs,addr,num);

}

void MainWindow::clearItems()
{

    //Clear items from registers model

    qDebug()<<  "MainWindow : clearItems" ;

    m_modbus->regModel->clear();
    ui->lblRegisters->setText("Registers > ");

}

void MainWindow::pollRequestForData()
{

    qDebug()<<  "MainWindow : pollRequestForData";
    request();

}

void MainWindow::request()
{

     //Request items from modbus adapter and add raw data to raw data model
    int rowCount = m_modbus->regModel->model->rowCount();

    if (rowCount == 0) {
        QMessageBox::critical(this, "Request failed","Add items to Registers Table.");
        return;
    }

    qDebug()<<  "MainWindow : requested registers = " <<  rowCount;


    const int slave = ui->sbSlaveID->value();
    const int functionCode = EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex());
    const int addr = ui->sbStartAddress->value();
    const int num = ui->sbNoOfCoils->value();

    //Modbus data
    m_modbus->modbusTransaction(slave,functionCode,addr,num);

    //Start-Stop poll timer
    if (ui->btRequest->isChecked() && ui->chkReqCycle->isChecked())
        m_pollTimer->start(ui->spInterval->value());
    else
        m_pollTimer->stop();

}

 void MainWindow::modbusConnect(bool connect)
 {

    //Modbus connect - RTU/TCP
    QString line ;
    qDebug()<<  "MainWindow : modbusConnect - " << connect;

    if (connect) {
        if (ui->cmbModbusMode->currentIndex() == EUtils::RTU) { //RTU
            line = "Connecting to Serial Port [" + m_modbusCommSettings->value("SerialPort").toString() + "]...";
            m_modbus->modbusConnectRTU(m_modbusCommSettings->value("SerialPortOS").toString(),
                                        m_modbusCommSettings->value("Baud").toInt(),
                                        EUtils::parity(m_modbusCommSettings->value("Parity").toString()),
                                        m_modbusCommSettings->value("DataBits").toInt(),
                                        m_modbusCommSettings->value("StopBits").toInt(),
                                        m_modbusCommSettings->value("RTS").toInt(),
                                        m_modbusCommSettings->value("TimeOut").toInt()
                                        );
            line += (m_modbus->isConnected() ? "OK" : "Failed");
        }
        else { //TCP
            line = "Connecting to TCP Port " + m_modbusCommSettings->value("SlaveIP").toString() + ":" + m_modbusCommSettings->value("TCPPort").toString() + "...";
            m_modbus->modbusConnectTCP(m_modbusCommSettings->value("SlaveIP").toString(),
                                       m_modbusCommSettings->value("TCPPort").toInt(),
                                       m_modbusCommSettings->value("TimeOut").toInt());
            line += (m_modbus->isConnected() ? "OK" : "Failed");
        }
        //Add line to raw data model
        line = EUtils::SysTimeStamp() + " : " + line;
        m_modbus->rawModel->addLine(line);
    }
    else { //Disconnect
        m_modbus->modbusDisConnect();
        ui->chkReqCycle->setChecked(false);
        m_pollTimer->stop();
        //Add line to raw data model
        line = EUtils::SysTimeStamp() + " : Disconnected";
        m_modbus->rawModel->addLine(line);
    }

    updateStatusBar();

    //Update buttons
    ui->btConnect->setChecked(m_modbus->isConnected());
    ui->btRequest->setCheckable(false);
    ui->btRequest->setEnabled(m_modbus->isConnected());
    ui->btRequest->repaint();
    ui->cmbModbusMode->setEnabled(!m_modbus->isConnected());
    ui->btAddItems->setEnabled(m_modbus->isConnected());

 }

 void MainWindow::refreshView()
 {

     qDebug()<<  "MainWindow : refrehView";
     ui->tblRegisters->resizeColumnsToContents();

 }
