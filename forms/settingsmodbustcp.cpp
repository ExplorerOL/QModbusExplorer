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
        ui->leIP1->setText(m_settings->slaveIPByte1());
        ui->leIP2->setText(m_settings->slaveIPByte2());
        ui->leIP3->setText(m_settings->slaveIPByte3());
        ui->leIP4->setText(m_settings->slaveIPByte4());
    }

}

void SettingsModbusTCP::changesAccepted()
{
    qDebug()<<  "SettingsModbusTCP : changes accepted" ;

    //Save Settings
    if (m_settings != NULL) {
        m_settings->setTCPPort(ui->leTCPPort->text());
        m_settings->setSlaveIPByte1(ui->leIP1->text());
        m_settings->setSlaveIPByte2(ui->leIP2->text());
        m_settings->setSlaveIPByte3(ui->leIP3->text());
        m_settings->setSlaveIPByte4(ui->leIP4->text());
    }

}
