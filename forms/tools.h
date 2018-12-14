#ifndef TOOLS_H
#define TOOLS_H

#include <QMainWindow>
#include <qcombobox.h>

#include "src/modbusadapter.h"
#include "src/modbuscommsettings.h"

namespace Ui {
class Tools;
}

class Tools : public QMainWindow
{
    Q_OBJECT

public:
    explicit Tools(QWidget *parent = 0, ModbusAdapter *adapter = 0, ModbusCommSettings *settings = 0);
    ~Tools();

private:
    Ui::Tools *ui;
    QComboBox *cmbModbusMode;
    QComboBox *cmbCmd;
    ModbusAdapter *m_modbus;
    ModbusCommSettings *m_modbusCommSettings;

private slots:
    void exit();
    void changedModbusMode(int currIndex);
    void execCmd();
    void Clear();

};

#endif // TOOLS_H
