#include "tools.h"
#include "ui_tools.h"

#include "QsLog.h"

Tools::Tools(QWidget *parent, ModbusAdapter *adapter, ModbusCommSettings *settings) :
    QMainWindow(parent),
    m_modbus(adapter), m_modbusCommSettings(settings),
    ui(new Ui::Tools)
{
    //setup UI
    ui->setupUi(this);
    cmbModbusMode = new QComboBox(this);
    cmbModbusMode->setMinimumWidth(96);
    cmbModbusMode->addItem("RTU");cmbModbusMode->addItem("TCP");
    cmbCmd = new QComboBox(this);
    cmbCmd->setMinimumWidth(96);
    cmbCmd->addItem("Diagnostic");
    ui->toolBar->addWidget(cmbModbusMode);
    ui->toolBar->addWidget(cmbCmd);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionExec);
    ui->toolBar->addAction(ui->actionClear);
    ui->toolBar->addAction(ui->actionExit);

    //UI - connections
    connect(cmbModbusMode,SIGNAL(currentIndexChanged(int)),this,SLOT(changedModbusMode(int)));
    connect(ui->actionExec,SIGNAL(triggered(bool)),this,SLOT(execCmd()));
    connect(ui->actionClear,SIGNAL(triggered(bool)),this,SLOT(clear()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exit()));
    connect(&m_pingProc,SIGNAL(readyReadStandardOutput()),this,SLOT(pingData()));
    connect(&m_pingProc,SIGNAL(readyReadStandardError()),this,SLOT(pingData()));

}

Tools::~Tools()
{
    delete ui;
}

void Tools::exit()
{

   this->close();

}

void Tools::changedModbusMode(int currIndex)
{

    QLOG_TRACE()<<  "Modbus Mode changed. Index = " << currIndex;

    cmbCmd->clear();
    if (currIndex == 0) { //RTU
       cmbCmd->addItem("Diagnostic");
    }
    else { //TCP
       cmbCmd->addItem("Diagnostic");cmbCmd->addItem("Ping");cmbCmd->addItem("Port Status");
    }

}

void Tools::execCmd()
{

    ui->txtOutput->moveCursor(QTextCursor::End);

    QLOG_TRACE()<<  "Tools Execute Cmd " << cmbCmd->currentText();
    switch (cmbCmd->currentIndex()){
        case 0:
        if (cmbModbusMode->currentIndex()==0)
           ui->txtOutput->appendPlainText(QString("------- Modbus RTU : Slave Addr %1 Diagnostics -------\n").arg(m_modbusCommSettings->slaveID()));
        else
           ui->txtOutput->appendPlainText(QString("------- Modbus TCP : Slave ID %1 Diagnostics -------\n").arg(m_modbusCommSettings->slaveID()));
        diagnosticsProc();
        break;

        case 1:
        ui->txtOutput->appendPlainText(QString("------- Modbus TCP : Ping IP %1 -------\n").arg(m_modbusCommSettings->slaveIP()));
        //m_pingProc.start("ping", QStringList() << m_modbusCommSettings->slaveIP());
        pingProc();
        break;

        case 2:
        ui->txtOutput->appendPlainText(QString("------- Modbus TCP : Check Port %1:%2 Status -------\n").arg(m_modbusCommSettings->slaveIP(),m_modbusCommSettings->TCPPort()));
        portProc();
        break;

        default:
        ui->txtOutput->appendPlainText("------- No Valid Selection -------\n");
        break;

    }

}

void Tools::clear()
{

    QLOG_TRACE()<<  "Tools Clear Ouput";
    ui->txtOutput->clear();

}

void Tools::diagnosticsProc()
{

    qDebug() << "Diagnostics";

}

void Tools::pingProc()
{

    qApp->processEvents();
    m_pingProc.start("ping", QStringList() << m_modbusCommSettings->slaveIP());
    if (m_pingProc.waitForFinished(5000)){
            //just wait -> execute button is pressed
    }
}

void Tools::pingData()
{

    qApp->processEvents();
    ui->txtOutput->moveCursor(QTextCursor::End);
    ui->txtOutput->insertPlainText(m_pingProc.readAll());

}

void Tools::portProc()
{

    qApp->processEvents();
    ui->txtOutput->moveCursor(QTextCursor::End);
    m_portProc.connectToHost(m_modbusCommSettings->slaveIP(),m_modbusCommSettings->TCPPort().toInt());
    if (m_portProc.waitForConnected(5000)){//wait -> execute button is pressed
        ui->txtOutput->insertPlainText("Connected.Port is opened\n");
        m_portProc.close();
    }
    else{
         ui->txtOutput->insertPlainText("Not connected.Port is closed\n");
    }

}
