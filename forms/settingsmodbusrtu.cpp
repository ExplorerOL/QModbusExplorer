#include <QtDebug>
#include "settingsmodbusrtu.h"
#include "ui_settingsmodbusrtu.h"

SettingsModbusRTU::SettingsModbusRTU(QWidget *parent,QSettings * settings) :
    QDialog(parent),
    ui(new Ui::SettingsModbusRTU),
    m_settings(settings)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(changesAccepted()));

}

SettingsModbusRTU::~SettingsModbusRTU()
{
    delete ui;
}

void SettingsModbusRTU::showEvent(QShowEvent * event)
{
    qWarning()<<  "SettingsModbusRTU : form show" ;

    //Load Settings
    if (m_settings != NULL) {

        ui->cmbPort->clear();

        //Populate cmbPort
        #ifdef Q_OS_WIN32
            ui->cmbPort->addItem("COM1:");
            ui->cmbPort->addItem("COM2:");
            ui->cmbPort->addItem("COM3:");
            ui->cmbPort->addItem("COM4:");
        #else
            ui->cmbPort->addItem("/dev/ttyS0");
            ui->cmbPort->addItem("/dev/ttyS1");
            ui->cmbPort->addItem("/dev/ttyS2");
            ui->cmbPort->addItem("/dev/ttyS3");
        #endif

        //Selection of port name
        if ((ui->cmbPort->findText(m_settings->value("SerialPort").toString()) == -1))
            ui->cmbPort->addItem(m_settings->value("SerialPort").toString());
        ui->cmbPort->addItem("Add Port");

        ui->cmbPort->setCurrentIndex(ui->cmbPort->findText(m_settings->value("SerialPort").toString()));
        ui->cmbBaud->setCurrentIndex(ui->cmbBaud->findText(m_settings->value("Baud").toString()));
        ui->cmbDataBits->setCurrentIndex(ui->cmbDataBits->findText(m_settings->value("DataBits").toString()));
        ui->cmbStopBits->setCurrentIndex(ui->cmbStopBits->findText(m_settings->value("StopBits").toString()));
        ui->cmbParity->setCurrentIndex(ui->cmbParity->findText(m_settings->value("Parity").toString()));
    }


}

void SettingsModbusRTU::changesAccepted()
{
    qWarning()<<  "SettingsModbusRTU : changes accepted" ;

    //Save Settings
    if (m_settings != NULL) {

        m_settings->setValue("SerialPort",ui->cmbPort->currentText());
        m_settings->setValue("SerialPortOS",ui->cmbPort->currentText());
        m_settings->setValue("Baud",ui->cmbBaud->currentText());
        m_settings->setValue("DataBits",ui->cmbDataBits->currentText());
        m_settings->setValue("StopBits",ui->cmbStopBits->currentText());
        m_settings->setValue("Parity",ui->cmbParity->currentText());
    }

}

