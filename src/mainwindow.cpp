#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>
#include <QtDebug>
#include <QMessageBox>
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

    //Update UI
    ui->sbNoOfCoils->setEnabled(true);
    ui->spInterval->setEnabled(false);
    ui->btRequest->setEnabled(false);
    updateStatusBar();

    //Init Code
    ui->tblRegisters->setModel(m_modbus->regModel->model);
    m_modbus->regModel->setBase(EUtils::Dec);
    //init MaxNoOfLines to 50 if is not defined yet
    m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->value("MaxNoOfLines").toInt() == 0 ? 50 : m_modbusCommSettings->value("MaxNoOfLines").toInt());
    //init TCP port to 502 if is not defined yet
    m_modbusCommSettings->setValue("TCPPort",(m_modbusCommSettings->value("TCPPort").toInt() == 0 ? 502 : m_modbusCommSettings->value("TCPPort").toInt()));
    //Poll Timer
    m_pollTimer=new QTimer(this);

    qWarning()<<  "MainWindow : Init Completed ";

}

MainWindow::~MainWindow()
{
    delete ui;

    qWarning()<<  "MainWindow : Closing" ;
}

void MainWindow::showSettingsModbusRTU()
{

    //Show RTU Settings Dialog

    if (m_dlgModbusRTU->exec()==QDialog::Accepted) {
        qWarning()<<  "MainWindow : RTU changes accepted ";
        updateStatusBar();
    }
    else
        qWarning()<<  "MainWindow : RTU changes rejected ";

}

void MainWindow::showSettingsModbusTCP()
{

    //Show TCP Settings Dialog

    if (m_dlgModbusTCP->exec()==QDialog::Accepted) {
        qWarning()<<  "MainWindow : TCP changes accepted ";
        updateStatusBar();
    }
    else
        qWarning()<<  "MainWindow : TCP changes rejected ";

}

void MainWindow::showSettings()
{

    //Show General Settings Dialog

    if (m_dlgSettings->exec()==QDialog::Accepted) {
        qWarning()<<  "MainWindow : changes accepted ";
        m_modbus->rawModel->setMaxNoOfLines(m_modbusCommSettings->value("MaxNoOfLines").toInt());
    }
    else
        qWarning()<<  "MainWindow : changes rejected ";

}

void MainWindow::showBusMonitor()
{

    //Show Bus Monitor

    qWarning()<<  "MainWindow : showBusMonitor ";

    m_busMonitor->show();

}

void MainWindow::changedModbusMode(int currIndex)
{

    //Show-Hide Slave IP

    qWarning()<<  "MainWindow : changedModbusMode index = " << currIndex;

    if (currIndex == 0) { //RTU

    }
    else { //TCP

    }

    updateStatusBar();

}

void MainWindow::changedFunctionCode(int currIndex)
{

    //Enable-Disable number of coils

    qWarning()<<  "MainWindow : changedFunctionCode index = " << currIndex;

    const int funcionCode = EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex());

    switch(funcionCode)
    {
        case _FC_WRITE_SINGLE_REGISTER:
        case _FC_WRITE_SINGLE_COIL:
                ui->sbNoOfCoils->setValue(1);
                ui->sbNoOfCoils->setEnabled(false);
                break;
        default:
                ui->sbNoOfCoils->setEnabled(true);
                break;
     }

    clearItems();

}

void MainWindow::changedBase(int currIndex)
{

    //Change Base

    qWarning()<<  "MainWindow : changedBase index = " << currIndex;

    switch(currIndex)
    {
        case 0:
                m_modbus->regModel->setBase(EUtils::Bin);
                break;
        case 1:
                m_modbus->regModel->setBase(EUtils::Dec);
                break;
        case 2:
                m_modbus->regModel->setBase(EUtils::Hex);
                break;
        default:
                m_modbus->regModel->setBase(EUtils::Hex);
                break;
     }


}

void MainWindow::changedReqCycle(bool value)
{

    //Enable-Disable Time Interval - Set-Reset btRequest As Toggle button

    qWarning()<<  "MainWindow : changedReqCycle value = " << value;

    ui->spInterval->setEnabled(value);
    ui->btRequest->setCheckable(value);
    ui->btRequest->repaint();

    //stop poll timer
    if (!value) m_pollTimer->stop();

}

void MainWindow::changedConnect(bool value)
{

    //Connect - Disconnect

    qWarning()<<  "MainWindow : changedConnect value = " << value;

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

    qWarning()<<  "MainWindow : updateStatusBar" ;

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
        qWarning()<<  "MainWindow : updateStatusBar - ind color = green" ;
    }
    else {
        m_statusInd->setStyleSheet("background-color: red");
        qWarning()<<  "MainWindow : updateStatusBar - ind color = red" ;
    }

}

void MainWindow::addItems()
{

    //Add items to registers model and to modbus adapter

    qWarning()<<  "MainWindow : addItems" ;
    qWarning()<<  "MainWindow : addItems - functionCode " << QString::number(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()),16);
    const QString dataType = EUtils::ModbusDataTypeName(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
    ui->lblRegisters->setText("Registers > " + dataType);
    bool valueIsEditable = EUtils::ModbusIsWriteFunction(EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex()));
    m_modbus->regModel->addItems(ui->sbStartAddress->text().toInt(),ui->sbNoOfCoils->text().toInt(),valueIsEditable);
    ui->tblRegisters->resizeColumnsToContents();

}

void MainWindow::clearItems()
{

    //Clear items from registers model

    qWarning()<<  "MainWindow : clearItems" ;

    m_modbus->regModel->clear();
    ui->lblRegisters->setText("Registers > ");

}

void MainWindow::pollRequestForData()
{

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

    qWarning()<<  "MainWindow : requested registers = " <<  rowCount;


    const int slave = ui->sbSlaveID->value();
    const int functionCode = EUtils::ModbusFunctionCode(ui->cmbFunctionCode->currentIndex());
    const int addr = ui->sbStartAddress->value();
    const int num = ui->sbNoOfCoils->value();

    //Request modbus data
    m_modbus->modbusRequestData(slave,functionCode,addr,num);

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
    qWarning()<<  "MainWindow : modbusConnect - " << connect;

    if (connect) {
        if (ui->cmbModbusMode->currentIndex() == EUtils::RTU) { //RTU
            line = "Connecting to Serial Port [" + m_modbusCommSettings->value("SerialPort").toString() + "]...";
            m_modbus->modbusConnectRTU(m_modbusCommSettings->value("SerialPortOS").toString(),
                                        m_modbusCommSettings->value("Baud").toInt(),
                                        EUtils::parity(m_modbusCommSettings->value("Parity").toString()),
                                        m_modbusCommSettings->value("DataBits").toInt(),
                                        m_modbusCommSettings->value("StopBits").toInt()
                                        );
            line += (m_modbus->isConnected() ? "OK" : "Failed");
        }
        else { //TCP
            line = "Connecting to TCP Port " + m_modbusCommSettings->value("SlaveIP").toString() + ":" + m_modbusCommSettings->value("TCPPort").toString() + "...";
            m_modbus->modbusConnectTCP(m_modbusCommSettings->value("SlaveIP").toString(),m_modbusCommSettings->value("TCPPort").toInt());
            line += (m_modbus->isConnected() ? "OK" : "Failed");
        }
        //Add line to raw data model
        line = EUtils::SysTimeStamp() + " : " + line;
        m_modbus->rawModel->addLine(line);
    }
    else { //Disconnect
        m_modbus->modbusDisConnect();
        //Add line to raw data model
        line = EUtils::SysTimeStamp() + " : Disconnected";
        m_modbus->rawModel->addLine(line);
    }

    updateStatusBar();

    //Update buttons
    ui->btConnect->setChecked(m_modbus->isConnected());
    ui->btRequest->setEnabled(m_modbus->isConnected());
    ui->cmbModbusMode->setEnabled(!m_modbus->isConnected());
    //start/stop polling
    if (m_modbus->isConnected())
        m_modbus->startPolling();
    else
        m_modbus->stopPolling();

 }
