#ifndef MODBUSCOMMSETTINGS_H
#define MODBUSCOMMSETTINGS_H

#include <QSettings>

class ModbusCommSettings : public QSettings
{
    Q_OBJECT
public:
    explicit ModbusCommSettings(QObject *parent = 0)  : QSettings(parent) { }
    ModbusCommSettings(const QString &fileName, Format format = QSettings::IniFormat, QObject *parent = 0);
    //TCP
    QString TCPPort();
    void setTCPPort(QString tcpPort);
    QString slaveIPByte1();
    void setSlaveIPByte1(QString IPByte);
    QString slaveIPByte2();
    void setSlaveIPByte2(QString IPByte);
    QString slaveIPByte3();
    void setSlaveIPByte3(QString IPByte);
    QString slaveIPByte4();
    void setSlaveIPByte4(QString IPByte);
    QString slaveIP();
    //Serial
    QString serialPort();
    void setSerialPort(QString serialPort);
    QString baud();
    void setBaud(QString baud);
    QString dataBits();
    void setDataBits(QString dataBits);
    QString stopBits();
    void setStopBits(QString stopBits);
    QString parity();
    void setParity(QString parity);
    QString RTS();
    void setRTS(QString RTS);
    //Var
    QString  maxNoOfLines();
    void setMaxNoOfLines(QString maxNoOfLines);
    QString  timeOut();
    void setTimeOut(QString timeOut);
    void loadSettings();
    void saveSettings();

private:
    //TCP
    QString m_TCPPort;
    QString m_slaveIPByte1;
    QString m_slaveIPByte2;
    QString m_slaveIPByte3;
    QString m_slaveIPByte4;
    //Serial
    QString m_serialPort;
    QString m_baud;
    QString m_dataBits;
    QString m_stopBits;
    QString m_parity;
    QString m_RTS;
    //Var
    QString m_maxNoOfLines;
    QString m_timeOut;

signals:

public slots:

};

#endif // MODBUSCOMMSETTINGS_H
