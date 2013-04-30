#include <QtDebug>
#include "settingsmodbustcp.h"
#include "ui_settingsmodbustcp.h"

SettingsModbusTCP::SettingsModbusTCP(QWidget *parent, ModbusCommSettings * settings) :
    QDialog(parent),
    ui(new Ui::SettingsModbusTCP),
    m_settings(settings)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(changesAccepted()));
}

SettingsModbusTCP::~SettingsModbusTCP()
{
    delete ui;
}

void SettingsModbusTCP::showEvent(QShowEvent * event)
{
    qDebug()<<  "SettingsModbusTCP : form show" ;

    //Load Settings
    if (m_settings != NULL) {
        ui->leTCPPort->setText(m_settings->TCPPort());
        ui->leSlaveIP->setText(m_settings->slaveIP());
    }

}

void SettingsModbusTCP::changesAccepted()
{
    qDebug()<<  "SettingsModbusTCP : changes accepted" ;

    //Save Settings
    if (m_settings != NULL) {
        m_settings->setTCPPort(ui->leTCPPort->text());
        m_settings->setSlaveIP(ui->leSlaveIP->text());
    }

}
