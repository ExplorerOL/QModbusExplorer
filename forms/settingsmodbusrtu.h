#ifndef SETTINGSMODBUSRTU_H
#define SETTINGSMODBUSRTU_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SettingsModbusRTU;
}

class SettingsModbusRTU : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsModbusRTU(QWidget *parent = 0 ,QSettings * settings = 0);
    ~SettingsModbusRTU();

private:
    Ui::SettingsModbusRTU *ui;
    QSettings * m_settings;

private slots:
    void changesAccepted();

protected:
    void showEvent(QShowEvent * event);

};

#endif // SETTINGSMODBUSRTU_H
