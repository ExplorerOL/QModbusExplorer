#include "tools.h"
#include "ui_tools.h"

#include "QsLog.h"

Tools::Tools(QWidget *parent, ModbusAdapter *adapter, ModbusCommSettings *settings) :
    QMainWindow(parent),
    m_modbus(adapter), m_modbusCommSettings(settings),
    ui(new Ui::Tools)
{
    ui->setupUi(this);
    cmbModbusMode = new QComboBox(this);
    cmbModbusMode->setMinimumWidth(96);
    cmbModbusMode->addItem("RTU");cmbModbusMode->addItem("TCP");
    cmbCmd = new QComboBox(this);
    cmbCmd->setMinimumWidth(96);
    cmbCmd->addItem("Diagnostic");
    ui->toolBar->addWidget(cmbModbusMode);
    ui->toolBar->addWidget(cmbCmd);
    ui->toolBar->addAction(ui->actionExec);
    ui->toolBar->addAction(ui->actionClear);
    ui->toolBar->addAction(ui->actionExit);
    connect(cmbModbusMode,SIGNAL(currentIndexChanged(int)),this,SLOT(changedModbusMode(int)));
    connect(ui->actionExec,SIGNAL(triggered(bool)),this,SLOT(execCmd()));
    connect(ui->actionClear,SIGNAL(triggered(bool)),this,SLOT(Clear()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exit()));
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

    QLOG_TRACE()<<  "Tools Execute Cmd " << cmbCmd->currentText();
    switch (cmbCmd->currentIndex()){
        case 0:
        if (cmbModbusMode->currentIndex()==0)
           ui->txtOutput->appendPlainText(QString("------- Modbus RTU : Slave Addr %1 Diagnostics -------").arg(m_modbusCommSettings->slaveID()));
        else
           ui->txtOutput->appendPlainText(QString("------- Modbus TCP : Slave ID %1 Diagnostics -------").arg(m_modbusCommSettings->slaveID()));
        break;

        case 1:
        ui->txtOutput->appendPlainText(QString("------- Modbus TCP : Ping IP %1 -------").arg(m_modbusCommSettings->slaveIP()));
        break;

        case 2:
        ui->txtOutput->appendPlainText(QString("------- Modbus TCP : Check Port %1:%2 Status -------").arg(m_modbusCommSettings->slaveIP(),m_modbusCommSettings->TCPPort()));
        break;

        default:
        ui->txtOutput->appendPlainText("------- No Valid Selection -------");
        break;

    }

}

void Tools::Clear()
{

    QLOG_TRACE()<<  "Tools Clear Ouput";
    ui->txtOutput->clear();

}
