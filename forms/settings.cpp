#include <QtDebug>
#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent ,ModbusCommSettings *settings) :
    QDialog(parent),
    ui(new Ui::Settings),
    m_settings(settings)
{
    ui->setupUi(this);
    parentWidget = parent;

    /* device name is needed only in Linux */
    #ifdef Q_OS_WIN32
        ui->cmbDev->setDisabled(true);
    #else
        ui->cmbDev->setDisabled(false);
    #endif

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(changesAccepted()));
    connect(ui->cmbModbusMode,SIGNAL(currentIndexChanged(int)),this,SLOT(updateSettingsUI(int)));
    //connect(ui->cmbLogLevel,  SIGNAL(currentIndexChanged(int)), this, SLOT(changeLogLevel(int)));

}

Settings::~Settings()
{
    delete ui;
}

void Settings::showEvent(QShowEvent * event)
{

    //Load Settings
    ui->cmbModbusMode->setCurrentIndex(m_settings->modbusMode());
    ui->sbMaxNoOfRawDataLines->setEnabled(!modbus_connected);
    ui->sbResponseTimeout->setEnabled(!modbus_connected);
    if (m_settings != NULL) {
        ui->sbMaxNoOfRawDataLines->setValue(m_settings->maxNoOfLines().toInt());
        ui->sbResponseTimeout->setValue(m_settings->timeOut().toInt());
        ui->sbBaseAddr->setValue(m_settings->baseAddr().toInt());
//        ui->cmbEndian->setCurrentIndex(m_settings->endian());

    }

    //Load Modbus RTU Settings
    ui->cmbDataBits->setEnabled(!modbus_connected);
    ui->cmbBaud->setEnabled(!modbus_connected);
    ui->sbPort->setEnabled(!modbus_connected);
    ui->cmbParity->setEnabled(!modbus_connected);
    ui->cmbRTS->setEnabled(!modbus_connected);
    ui->cmbStopBits->setEnabled(!modbus_connected);

    //Load Modbus TCP Settings
    ui->leSlaveIP->setEnabled(!modbus_connected);
    ui->leTCPPort->setEnabled(!modbus_connected);
    if (m_settings != NULL) {
        ui->leTCPPort->setText(m_settings->TCPPort());
        ui->leSlaveIP->setText(m_settings->slaveIP());
    }



    //ui->gbRTUSettings -> hide();

    if (m_settings != NULL) {

         ui->cmbRTS->clear();

        //Populate cmbPort-cmbRTS
        #ifdef Q_OS_WIN32
            ui->cmbRTS->addItem("Disable");
            ui->cmbRTS->addItem("Enable");
            ui->cmbRTS->addItem("HandShake");
            ui->cmbRTS->addItem("Toggle");
        #else
            ui->cmbRTS->addItem("None");
            ui->cmbRTS->addItem("Up");
            ui->cmbRTS->addItem("Down");
        #endif

        ui->cmbDev->setCurrentText(m_settings->serialDev());
        ui->sbPort->setValue(m_settings->serialPort().toInt());
        ui->cmbBaud->setCurrentIndex(ui->cmbBaud->findText(m_settings->baud()));
        ui->cmbDataBits->setCurrentIndex(ui->cmbDataBits->findText(m_settings->dataBits()));
        ui->cmbStopBits->setCurrentIndex(ui->cmbStopBits->findText(m_settings->stopBits()));
        ui->cmbParity->setCurrentIndex(ui->cmbParity->findText(m_settings->parity()));
        ui->cmbRTS->setCurrentIndex(ui->cmbRTS->findText(m_settings->RTS()));

        ui->cmbLogLevel->setCurrentIndex(m_settings->loggingLevel());
        updateSettingsUI(m_settings->modbusMode());



    //parentWidget->chan
    }
}

void Settings::changesAccepted()
{

    //Save Settings
    if (m_settings != NULL) {
        m_settings->setModbusMode(ui->cmbModbusMode->currentIndex());
        m_settings->setMaxNoOfLines(ui->sbMaxNoOfRawDataLines->cleanText());
        m_settings->setTimeOut(ui->sbResponseTimeout->cleanText());
        m_settings->setBaseAddr(ui->sbBaseAddr->cleanText());
        m_settings->setLoggingLevel(ui->cmbLogLevel->currentIndex());
//        if (m_settings->endian() != ui->cmbEndian->currentIndex())
//            emit changedEndianess(ui->cmbEndian->currentIndex());
//        m_settings->setEndian(ui->cmbEndian->currentIndex());

         //from settingsmodbusrtu.cpp
        m_settings->setSerialPort(QString::number(ui->sbPort->value()), ui->cmbDev->currentText());
        m_settings->setBaud(ui->cmbBaud->currentText());
        m_settings->setDataBits(ui->cmbDataBits->currentText());
        m_settings->setStopBits(ui->cmbStopBits->currentText());
        m_settings->setParity(ui->cmbParity->currentText());
        m_settings->setRTS((QString)ui->cmbRTS->currentText());

    int validation;

    validation = validateInputs();
    switch(validation){
        case 0 : // ok
            //Save Settings
            if (m_settings != NULL) {
                m_settings->setTCPPort(ui->leTCPPort->text());
                m_settings->setSlaveIP(ui->leSlaveIP->text());
            }
            break;
        case 1 : // wrong ip
            QMessageBox::critical(NULL, "Modbus TCP Settings","Wrong IP Address.");
            break;
        case 2 : // wrong port
            QMessageBox::critical(NULL, "Modbus TCP Settings","Wrong Port Number.");
            break;
    }






    }

}


int Settings::validateInputs()
{
    //Strip zero's from IP
    QStringList ipBytes;
    bool ok;
    int i, ipByte, port;

    ipBytes = (ui->leSlaveIP->text()).split(".");
    if (ipBytes.size() == 4){
        for (i = 0; i < ipBytes.size(); i++){
            ipByte = ipBytes[i].toInt(&ok);
            if (!ok || ipByte > 255 )
                return 1; // wrong ip
        }
    }
    else
        return 1; // wrong ip

    port = (ui->leTCPPort->text()).toInt(&ok);
    if (!ok || port <= 0 || port > 65535)
        return 2; // wrong port

    return 0; // validate ok

}

void Settings::updateSettingsUI(int index) {
    if (index == 0) {
            ui->gbRTUSettings -> show();
            ui->gbTCPSettings -> hide();
    } else {
            ui->gbRTUSettings -> hide();
            ui->gbTCPSettings -> show();
    }
    this->adjustSize();
}


