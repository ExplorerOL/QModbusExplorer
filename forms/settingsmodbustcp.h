#ifndef SETTINGSMODBUSTCP_H
#define SETTINGSMODBUSTCP_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SettingsModbusTCP;
}

class SettingsModbusTCP : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsModbusTCP(QWidget *parent = 0 ,QSettings * settings = 0);
    ~SettingsModbusTCP();

private:
    Ui::SettingsModbusTCP *ui;
    QSettings * m_settings;

private slots:
    void changesAccepted();


protected:
    void showEvent(QShowEvent * event);

};

#endif // SETTINGSMODBUSTCP_H
