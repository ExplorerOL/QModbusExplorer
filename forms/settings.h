#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QMessageBox>
#include <QWidget>

#include "src/modbuscommsettings.h"

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0 ,ModbusCommSettings * settings = 0);
    ~Settings();
    bool modbus_connected;

private:
    Ui::Settings *ui;
    ModbusCommSettings *m_settings;
    int validateInputs();
    QWidget *parentWidget;

signals:
    void changedEndianess(int endian);

private slots:
    void changesAccepted();
    void updateSettingsUI(int);

    //from settingsmodbusrtu.h
    //void changesAccepted();


protected:
    void showEvent(QShowEvent * event);

};

#endif // SETTINGS_H
