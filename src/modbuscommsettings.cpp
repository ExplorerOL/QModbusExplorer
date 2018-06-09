#include "modbuscommsettings.h"
#include "QsLog.h"

ModbusCommSettings::ModbusCommSettings(const QString &fileName, Format format , QObject *parent)
    : QSettings(fileName, format, parent)
{
    this->loadSettings();
}

void  ModbusCommSettings::loadSettings()
{

    if (this->value("TCP/TCPPort").isNull())
        m_TCPPort = "502";
    else
        m_TCPPort = this->value("TCP/TCPPort").toString();

    if (this->value("TCP/SlaveIP").isNull())
        m_slaveIP = "127.000.000.001";
    else
        m_slaveIP = this->value("TCP/SlaveIP").toString();

    if (this->value("RTU/SerialDev").isNull())
        m_serialDev = "/dev/ttyS";
    else
        m_serialDev = this->value("RTU/SerialDev").toString();

    if (this->value("RTU/SerialPort").isNull())
    {
        m_serialPort = "1";
        #ifdef Q_OS_WIN32
            m_serialPortName = "COM" + m_serialPort;
        #else
            m_serialPortName = m_serialDev;
            m_serialPortName += QStringLiteral("%1").arg(m_serialPort.toInt() - 1);
        #endif
    }
    else {
        m_serialPort = this->value("RTU/SerialPort").toString();
        m_serialPortName = this->value("RTU/SerialPortName").toString();
    }

    if (this->value("RTU/Baud").isNull())
        m_baud = "9600";
    else
        m_baud = this->value("RTU/Baud").toString();

    if (this->value("RTU/DataBits").isNull())
        m_dataBits = "8";
    else
        m_dataBits = this->value("RTU/DataBits").toString();

    if (this->value("RTU/StopBits").isNull())
        m_stopBits = "1";
    else
        m_stopBits = this->value("RTU/StopBits").toString();

    if (this->value("RTU/Parity").isNull())
        m_parity = "None";
    else
        m_parity = this->value("RTU/Parity").toString();

    if (this->value("RTU/RTS").isNull())
        #ifdef Q_OS_WIN32
            m_RTS = "Disable";
        #else
            m_RTS = "None";
        #endif
    else
        m_RTS = this->value("RTU/RTS").toString();

    if (this->value("Var/MaxNoOfLines").toInt() == 0 ||
            this->value("Var/MaxNoOfLines").isNull())
        m_maxNoOfLines = "60";
    else
        m_maxNoOfLines = this->value("Var/MaxNoOfLines").toString();

    if (this->value("Var/BaseAddr").isNull())
        m_baseAddr = "0";
    else
        m_baseAddr = this->value("Var/BaseAddr").toString();

    if (this->value("Var/TimeOut").isNull())
        m_timeOut = "0";
    else
        m_timeOut = this->value("Var/TimeOut").toString();

    if (this->value("Var/LoggingLevel").isNull())
        m_loggingLevel = 3; //warning level
    else
        m_loggingLevel = this->value("Var/LoggingLevel").toInt();

    if (this->value("Session/ModBusMode").isNull())
        m_modbusMode = 0; //RTU
    else
        m_modbusMode = this->value("Session/ModBusMode").toInt();

    if (this->value("Session/SlaveID").isNull())
        m_slaveID = 1;
    else
        m_slaveID = this->value("Session/SlaveID").toInt();

    if (this->value("Session/ScanRate").isNull())
        m_scanRate = 1000;
    else
        m_scanRate = this->value("Session/ScanRate").toInt();

    if (this->value("Session/FunctionCode").isNull())
        m_functionCode = 0; //FC1 : Read Coils
    else
        m_functionCode = this->value("Session/FunctionCode").toInt();

    if (this->value("Session/StartAddr").isNull())
        m_startAddr = 0;
    else
        m_startAddr = this->value("Session/StartAddr").toInt();

    if (this->value("Session/NoOfRegs").isNull())
        m_noOfRegs = 0;
    else
        m_noOfRegs = this->value("Session/NoOfRegs").toInt();

    if (this->value("Session/Base").isNull())
        m_base = 1; //Dec
    else
        m_base = this->value("Session/Base").toInt();

}

void  ModbusCommSettings::saveSettings()
{

    this->setValue("TCP/TCPPort",m_TCPPort);
    this->setValue("TCP/SlaveIP",m_slaveIP);
    this->setValue("RTU/SerialDev",m_serialDev);
    this->setValue("RTU/SerialPort",m_serialPort);
    this->setValue("RTU/SerialPortName",m_serialPortName);
    this->setValue("RTU/Baud",m_baud);
    this->setValue("RTU/DataBits",m_dataBits);
    this->setValue("RTU/StopBits",m_stopBits);
    this->setValue("RTU/Parity",m_parity);
    this->setValue("RTU/RTS",m_RTS);
    this->setValue("Var/MaxNoOfLines",m_maxNoOfLines);
    this->setValue("Var/BaseAddr",m_baseAddr);
    this->setValue("Var/TimeOut",m_timeOut);
    this->setValue("Var/LoggingLevel",m_loggingLevel);
    this->setValue("Session/ModBusMode",m_modbusMode);
    this->setValue("Session/SlaveID",m_slaveID);
    this->setValue("Session/ScanRate",m_scanRate);
    this->setValue("Session/FunctionCode",m_functionCode);
    this->setValue("Session/StartAddr",m_startAddr);
    this->setValue("Session/NoOfRegs",m_noOfRegs);
    this->setValue("Session/Base",m_base);

}

QString  ModbusCommSettings::TCPPort()
{
    return m_TCPPort;
}

void ModbusCommSettings::setTCPPort(QString tcpPort)
{
    m_TCPPort = tcpPort;
}

void ModbusCommSettings::setSlaveIP(QString IP)
{
    m_slaveIP = IP;
}

QString  ModbusCommSettings::slaveIP()
{
    return m_slaveIP;
}

QString  ModbusCommSettings::serialDev()
{
    return m_serialDev;
}

QString  ModbusCommSettings::serialPort()
{
    return m_serialPort;
}

QString  ModbusCommSettings::serialPortName()
{
    return m_serialPortName;
}

void ModbusCommSettings::setSerialPort(QString serialPort, QString serialDev)
{
int serialPortNo;

    m_serialDev = serialDev;
    m_serialPort = serialPort;
    serialPortNo = serialPort.toInt();
    #ifdef Q_OS_WIN32
    if (serialPortNo > 9)
        m_serialPortName = "\\\\.\\COM" + serialPort;
    else
        m_serialPortName = "COM" + serialPort;
    #else
        m_serialPortName = serialDev;
        m_serialPortName += QStringLiteral("%1").arg(serialPort.toInt() - 1);
    #endif
}

QString  ModbusCommSettings::baud()
{
    return m_baud;
}

void ModbusCommSettings::setBaud(QString baud)
{
    m_baud = baud;
}

QString  ModbusCommSettings::dataBits()
{
    return m_dataBits;
}

void ModbusCommSettings::setDataBits(QString dataBits)
{
    m_dataBits = dataBits;
}

QString  ModbusCommSettings::stopBits()
{
    return m_stopBits;
}

void ModbusCommSettings::setStopBits(QString stopBits)
{
    m_stopBits = stopBits;
}

QString  ModbusCommSettings::parity()
{
    return m_parity;
}

void ModbusCommSettings::setParity(QString parity)
{
    m_parity = parity;
}

QString  ModbusCommSettings::RTS()
{
    return m_RTS;
}

void ModbusCommSettings::setRTS(QString RTS)
{
    m_RTS = RTS;
}

QString  ModbusCommSettings::maxNoOfLines()
{
    return m_maxNoOfLines;
}

void ModbusCommSettings::setMaxNoOfLines(QString maxNoOfLines)
{
    m_maxNoOfLines = maxNoOfLines;
}

QString  ModbusCommSettings::baseAddr()
{
    return m_baseAddr;
}

void ModbusCommSettings::setBaseAddr(QString baseAddr)
{
    m_baseAddr = baseAddr;
}
QString  ModbusCommSettings::timeOut()
{
    return m_timeOut;
}

void ModbusCommSettings::setTimeOut(QString timeOut)
{
    m_timeOut = timeOut;
}

int  ModbusCommSettings::loggingLevel()
{
    return m_loggingLevel;
}

int ModbusCommSettings::modbusMode()
{
    return m_modbusMode;
}

void ModbusCommSettings::setModbusMode(int modbusMode)
{
    m_modbusMode = modbusMode;
}

int ModbusCommSettings::slaveID()
{
    return m_slaveID;
}

void ModbusCommSettings::setSlaveID(int slaveID)
{
    m_slaveID = slaveID;
}

int ModbusCommSettings::scanRate()
{
    return m_scanRate;
}

void ModbusCommSettings::setScanRate(int scanRate)
{
    m_scanRate = scanRate;
}

int ModbusCommSettings::functionCode()
{
    return m_functionCode;
}

void ModbusCommSettings::setFunctionCode(int functionCode)
{
    m_functionCode = functionCode;
}

int ModbusCommSettings::startAddr()
{
    return m_startAddr;
}

void ModbusCommSettings::setStartAddr(int startAddr)
{
    m_startAddr = startAddr;
}

int ModbusCommSettings::noOfRegs()
{
    return m_noOfRegs;
}

void ModbusCommSettings::setNoOfRegs(int noOfRegs)
{
    m_noOfRegs = noOfRegs;
}

int ModbusCommSettings::base()
{
    return m_base;
}

void ModbusCommSettings::setBase(int base)
{
    m_base = base;
}

void ModbusCommSettings::loadSession(QString fName)
{

    QLOG_INFO()<<  "Load session config from file " << fName;
    QSettings m_save_session(fName, QSettings::IniFormat, this);
    if (m_save_session.value("TCP/TCPPort").isNull())
        m_TCPPort = "502";
    else
        m_TCPPort = m_save_session.value("TCP/TCPPort").toString();

    if (m_save_session.value("TCP/SlaveIP").isNull())
        m_slaveIP = "127.000.000.001";
    else
        m_slaveIP = m_save_session.value("TCP/SlaveIP").toString();

    if (m_save_session.value("RTU/SerialDev").isNull())
        m_serialDev = "/dev/ttyS";
    else
        m_serialDev = m_save_session.value("RTU/SerialDev").toString();

    if (m_save_session.value("RTU/SerialPort").isNull())
    {
        m_serialPort = "1";
        #ifdef Q_OS_WIN32
            m_serialPortName = "COM" + m_serialPort;
        #else
            m_serialPortName = m_serialDev;
            m_serialPortName += QStringLiteral("%1").arg(m_serialPort.toInt() - 1);
        #endif
    }
    else {
        m_serialPort = m_save_session.value("RTU/SerialPort").toString();
        m_serialPortName = m_save_session.value("RTU/SerialPortName").toString();
    }

    if (m_save_session.value("RTU/Baud").isNull())
        m_baud = "9600";
    else
        m_baud = m_save_session.value("RTU/Baud").toString();

    if (m_save_session.value("RTU/DataBits").isNull())
        m_dataBits = "8";
    else
        m_dataBits = m_save_session.value("RTU/DataBits").toString();

    if (m_save_session.value("RTU/StopBits").isNull())
        m_stopBits = "1";
    else
        m_stopBits = m_save_session.value("RTU/StopBits").toString();

    if (m_save_session.value("RTU/Parity").isNull())
        m_parity = "None";
    else
        m_parity = m_save_session.value("RTU/Parity").toString();

    if (m_save_session.value("RTU/RTS").isNull())
        #ifdef Q_OS_WIN32
            m_RTS = "Disable";
        #else
            m_RTS = "None";
        #endif
    else
        m_RTS = m_save_session.value("RTU/RTS").toString();

    if (m_save_session.value("Var/MaxNoOfLines").toInt() == 0 ||
            m_save_session.value("Var/MaxNoOfLines").isNull())
        m_maxNoOfLines = "60";
    else
        m_maxNoOfLines = m_save_session.value("Var/MaxNoOfLines").toString();

    if (m_save_session.value("Var/BaseAddr").isNull())
        m_baseAddr = "0";
    else
        m_baseAddr = m_save_session.value("Var/BaseAddr").toString();

    if (m_save_session.value("Var/TimeOut").isNull())
        m_timeOut = "0";
    else
        m_timeOut = m_save_session.value("Var/TimeOut").toString();

    if (m_save_session.value("Var/LoggingLevel").isNull())
        m_loggingLevel = 3; //warning level
    else
        m_loggingLevel = m_save_session.value("Var/LoggingLevel").toInt();

    if (m_save_session.value("Session/ModBusMode").isNull())
        m_modbusMode = 0; //RTU
    else
        m_modbusMode = m_save_session.value("Session/ModBusMode").toInt();

    if (m_save_session.value("Session/SlaveID").isNull())
        m_slaveID = 1;
    else
        m_slaveID = m_save_session.value("Session/SlaveID").toInt();

    if (m_save_session.value("Session/ScanRate").isNull())
        m_scanRate = 1000;
    else
        m_scanRate = m_save_session.value("Session/ScanRate").toInt();

    if (m_save_session.value("Session/FunctionCode").isNull())
        m_functionCode = 0; //FC1 : Read Coils
    else
        m_functionCode = m_save_session.value("Session/FunctionCode").toInt();

    if (m_save_session.value("Session/StartAddr").isNull())
        m_startAddr = 0;
    else
        m_startAddr = m_save_session.value("Session/StartAddr").toInt();

    if (m_save_session.value("Session/NoOfRegs").isNull())
        m_noOfRegs = 0;
    else
        m_noOfRegs = m_save_session.value("Session/NoOfRegs").toInt();

    if (m_save_session.value("Session/Base").isNull())
        m_base = 1; //Dec
    else
        m_base = m_save_session.value("Session/Base").toInt();

}

void ModbusCommSettings::saveSession(QString fName)
{

    QLOG_INFO()<<  "Save session config to file " << fName;
    QSettings m_save_session(fName, QSettings::IniFormat, this);
    m_save_session.setValue("TCP/TCPPort",m_TCPPort);
    m_save_session.setValue("TCP/SlaveIP",m_slaveIP);
    m_save_session.setValue("RTU/SerialDev",m_serialDev);
    m_save_session.setValue("RTU/SerialPort",m_serialPort);
    m_save_session.setValue("RTU/SerialPortName",m_serialPortName);
    m_save_session.setValue("RTU/Baud",m_baud);
    m_save_session.setValue("RTU/DataBits",m_dataBits);
    m_save_session.setValue("RTU/StopBits",m_stopBits);
    m_save_session.setValue("RTU/Parity",m_parity);
    m_save_session.setValue("RTU/RTS",m_RTS);
    m_save_session.setValue("Var/MaxNoOfLines",m_maxNoOfLines);
    m_save_session.setValue("Var/BaseAddr",m_baseAddr);
    m_save_session.setValue("Var/TimeOut",m_timeOut);
    m_save_session.setValue("Var/LoggingLevel",m_loggingLevel);
    m_save_session.setValue("Session/ModBusMode",m_modbusMode);
    m_save_session.setValue("Session/SlaveID",m_slaveID);
    m_save_session.setValue("Session/ScanRate",m_scanRate);
    m_save_session.setValue("Session/FunctionCode",m_functionCode);
    m_save_session.setValue("Session/StartAddr",m_startAddr);
    m_save_session.setValue("Session/NoOfRegs",m_noOfRegs);
    m_save_session.setValue("Session/Base",m_base);
    m_save_session.sync();

}
