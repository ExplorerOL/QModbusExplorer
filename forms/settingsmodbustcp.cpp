#include <QtDebug>
#include "settingsmodbustcp.h"
#include "ui_settingsmodbustcp.h"

SettingsModbusTCP::SettingsModbusTCP(QWidget *parent, QSettings * settings) :
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
        ui->leTCPPort->setText(m_settings->value("TCPPort").toString());
        ui->leIP1->setText(m_settings->value("SlaveIPByte1").toString());
        ui->leIP2->setText(m_settings->value("SlaveIPByte2").toString());
        ui->leIP3->setText(m_settings->value("SlaveIPByte3").toString());
        ui->leIP4->setText(m_settings->value("SlaveIPByte4").toString());
    }

}

void SettingsModbusTCP::changesAccepted()
{
    qDebug()<<  "SettingsModbusTCP : changes accepted" ;

    //Save Settings
    if (m_settings != NULL) {
        m_settings->setValue("TCPPort",ui->leTCPPort->text());
        m_settings->setValue("SlaveIPByte1",ui->leIP1->text());
        m_settings->setValue("SlaveIPByte2",ui->leIP2->text());
        m_settings->setValue("SlaveIPByte3",ui->leIP3->text());
        m_settings->setValue("SlaveIPByte4",ui->leIP4->text());
        m_settings->setValue("SlaveIP",(ui->leIP1->text() + "." + ui->leIP2->text() + "." +
                                        ui->leIP3->text() + "." + ui->leIP4->text()));
    }

}
