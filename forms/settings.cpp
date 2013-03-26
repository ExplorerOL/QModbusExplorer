#include <QtDebug>
#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent ,ModbusCommSettings * settings) :
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
    qDebug()<<  "Settings : form show" ;

    //Load Settings
    if (m_settings != NULL) {
        ui->sbMaxNoOfRawDataLines->setValue(m_settings->maxNoOfLines().toInt());
        ui->sbResponseTimeout->setValue(m_settings->timeOut().toInt());
    }

}

void Settings::changesAccepted()
{
    qDebug()<<  "Settings : changes accepted" ;

    //Save Settings
    if (m_settings != NULL) {
        m_settings->setMaxNoOfLines((QString)ui->sbMaxNoOfRawDataLines->value());
        m_settings->setTimeOut((QString)ui->sbResponseTimeout->value());
    }

}
