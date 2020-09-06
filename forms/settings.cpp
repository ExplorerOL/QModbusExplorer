#include <QtDebug>
#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent ,ModbusCommSettings *settings) :
    QDialog(parent),
    ui(new Ui::Settings),
    m_settings(settings)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(changesAccepted()));

}

Settings::~Settings()
{
    delete ui;
}

void Settings::showEvent(QShowEvent * event)
{

    //Load Settings
    ui->sbMaxNoOfRawDataLines->setEnabled(!modbus_connected);
    ui->sbResponseTimeout->setEnabled(!modbus_connected);
    if (m_settings != NULL) {
        ui->sbMaxNoOfRawDataLines->setValue(m_settings->maxNoOfLines().toInt());
        ui->sbResponseTimeout->setValue(m_settings->timeOut().toInt());
        ui->sbBaseAddr->setValue(m_settings->baseAddr().toInt());
        ui->cmbEndian->setCurrentIndex(m_settings->endian());
    }

}

void Settings::changesAccepted()
{

    //Save Settings
    if (m_settings != NULL) {
        m_settings->setMaxNoOfLines(ui->sbMaxNoOfRawDataLines->cleanText());
        m_settings->setTimeOut(ui->sbResponseTimeout->cleanText());
        m_settings->setBaseAddr(ui->sbBaseAddr->cleanText());
        if (m_settings->endian() != ui->cmbEndian->currentIndex())
            emit changedEndianess(ui->cmbEndian->currentIndex());
        m_settings->setEndian(ui->cmbEndian->currentIndex());
    }

}
