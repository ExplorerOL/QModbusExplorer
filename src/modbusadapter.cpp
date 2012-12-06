#include "modbusadapter.h"
#include <QMessageBox>
#include <QtDebug>
#include <errno.h>

ModbusAdapter *m_instance;

#ifdef QT_NO_DEBUG_OUTPUT
    #define DBG 0
#else
    #define DBG 1
#endif

ModbusAdapter::ModbusAdapter(QObject *parent) :
    QObject(parent),
    m_modbus(NULL)
{
    m_instance=this;
    regModel=new RegistersModel(this);
    rawModel=new RawDataModel(this);
    m_connected = false;
    m_ModBusMode = EUtils::None;
}

void ModbusAdapter::modbusConnectRTU(QString port, int baud, QChar parity, int dataBits, int stopBits, int RTS, int timeOut)
{
    //Modbus RTU connect

    modbusDisConnect();

    m_modbus = modbus_new_rtu(port.toAscii().constData(),baud,parity.toAscii(),dataBits,stopBits,RTS);


    //Debug messages from libmodbus
    modbus_set_debug(m_modbus, DBG);
    //Response timeout
    struct timeval response_timeout;
    response_timeout.tv_sec = timeOut;
    response_timeout.tv_usec = 0;
    modbus_set_response_timeout(m_modbus, &response_timeout);

    if(m_modbus && modbus_connect(m_modbus) == -1) {
        QMessageBox::critical(NULL, "Connection failed", "Could not connect to serial port!");
        m_connected = false;
    }
    else
        m_connected = true;

    m_ModBusMode = EUtils::RTU;

}

void ModbusAdapter::modbusConnectTCP(QString ip, int port, int timeOut)
{
    //Modbus RTU connect

    modbusDisConnect();

    qDebug()<<  "ModbusAdapter : modbusConnect TCP";

    m_modbus = modbus_new_tcp(ip.toAscii().constData(), port);

    //Debug messages from libmodbus
    modbus_set_debug(m_modbus, DBG);
    //Response timeout
    struct timeval response_timeout;
    response_timeout.tv_sec = timeOut;
    response_timeout.tv_usec = 0;
    modbus_set_response_timeout(m_modbus, &response_timeout);

    if(m_modbus && modbus_connect(m_modbus) == -1) {
        QMessageBox::critical(NULL, "Connection failed", "Could not connect to TCP port!");
        m_connected = false;
    }
    else
        m_connected = true;

    m_ModBusMode = EUtils::TCP;

}


void ModbusAdapter::modbusDisConnect()
{
    //Modbus disconnect

    qDebug()<<  "ModbusAdapter : modbusDisConnect ";

    if(m_modbus) {
        modbus_close(m_modbus);
        modbus_free(m_modbus);
        m_modbus = NULL;
    }

    m_connected = false;

    m_ModBusMode = EUtils::None;

}

bool ModbusAdapter::isConnected()
{
    //Modbus is connected

    return m_connected;
}

void ModbusAdapter::modbusTransaction(int slave, int functionCode, int startAddress, int noOfItems)
{
    //Modbus request data

    qDebug()<<  "ModbusAdapter : modbusTransaction ";

    switch(functionCode)
    {
            case _FC_READ_COILS:
            case _FC_READ_DISCRETE_INPUTS:
            case _FC_READ_HOLDING_REGISTERS:
            case _FC_READ_INPUT_REGISTERS:
                    modbusReadData(slave,functionCode,startAddress,noOfItems);
                    break;

            case _FC_WRITE_SINGLE_COIL:
            case _FC_WRITE_SINGLE_REGISTER:
            case _FC_WRITE_MULTIPLE_COILS:
            case _FC_WRITE_MULTIPLE_REGISTERS:
                    modbusWriteData(slave,functionCode,startAddress,noOfItems);
                    break;
            default:
                    break;
    }

    emit(refreshView());

}

void ModbusAdapter::modbusReadData(int slave, int functionCode, int startAddress, int noOfItems)
{

    qDebug()<<  "ModbusAdapter : modbusReadData ";

    if(m_modbus == NULL) return;

    uint8_t dest[1024]; //setup memory for data
    uint16_t * dest16 = (uint16_t *) dest;
    memset(dest, 0, 1024);
    int ret = -1; //return value from read functions
    bool is16Bit = false;

    modbus_set_slave(m_modbus, slave);
    //request data from modbus
    switch(functionCode)
    {
            case _FC_READ_COILS:
                    ret = modbus_read_bits(m_modbus, startAddress, noOfItems, dest);
                    break;

            case _FC_READ_DISCRETE_INPUTS:
                    ret = modbus_read_input_bits(m_modbus, startAddress, noOfItems, dest);
                    break;

            case _FC_READ_HOLDING_REGISTERS:
                    ret = modbus_read_registers(m_modbus, startAddress, noOfItems, dest16);
                    is16Bit = true;
                    break;

            case _FC_READ_INPUT_REGISTERS:
                    ret = modbus_read_input_registers(m_modbus, startAddress, noOfItems, dest16);
                    is16Bit = true;
                    break;

            default:
                    break;
    }

    qDebug()<<  "ModbusAdapter : modbusReadData ret = " << ret;

    //update data model
    if(ret == noOfItems)
    {
            for(int i = 0; i < noOfItems; ++i)
            {
                int data = is16Bit ? dest16[i] : dest[i];
                regModel->setValue(i,data);
            }
    }
    else
    {

        regModel->setNoValidValues();

        QString line;
        if(ret < 0) {
                line = QString("Slave threw exception  >  ").arg(ret) +  modbus_strerror(errno) + " ";
                qDebug()<<  "ModbusAdapter : modbusRequestData - " << line;
                rawModel->addLine(EUtils::SysTimeStamp() + " : " + line);
        }
        else {
                line = QString("Number of registers returned does not match number of registers requested!. [")  +  modbus_strerror(errno) + "]";
                qDebug()<<  "ModbusAdapter : modbusRequestData - " << "Number of registers returned does not match number of registers requested!";
                rawModel->addLine(EUtils::SysTimeStamp() + " : " + line);
        }
     }

}

void ModbusAdapter::modbusWriteData(int slave, int functionCode, int startAddress, int noOfItems)
{

    qDebug()<<  "ModbusAdapter : modbusWriteData ";

    if(m_modbus == NULL) return;

    int ret = -1; //return value from functions

    modbus_set_slave(m_modbus, slave);
    //request data from modbus
    switch(functionCode)
    {
            case _FC_WRITE_SINGLE_COIL:
                    ret = modbus_write_bit(m_modbus, startAddress,regModel->value(0));
                    noOfItems = 1;
                    break;

            case _FC_WRITE_SINGLE_REGISTER:
                    ret = modbus_write_register( m_modbus, startAddress,regModel->value(0));
                    noOfItems = 1;
                    break;

            case _FC_WRITE_MULTIPLE_COILS:
            {
                    uint8_t * data = new uint8_t[noOfItems];
                    for(int i = 0; i < noOfItems; ++i)
                    {
                            data[i] = regModel->value(i);
                    }
                    ret = modbus_write_bits(m_modbus, startAddress, noOfItems, data);
                    delete[] data;
                    break;
            }
            case _FC_WRITE_MULTIPLE_REGISTERS:
            {
                    uint16_t * data = new uint16_t[noOfItems];
                    for(int i = 0; i < noOfItems; ++i)
                    {
                            data[i] = regModel->value(i);
                    }
                    ret = modbus_write_registers(m_modbus, startAddress, noOfItems, data);
                    delete[] data;
                    break;
            }

            default:
                    break;
    }

    qDebug()<<  "ModbusAdapter : modbusWriteData ret = " << ret;

    //update data model
    if(ret == noOfItems)
    {
        //values written correctly
        rawModel->addLine(EUtils::SysTimeStamp() + " : values written correctly.");
    }
    else
    {

        regModel->setNoValidValues();

        QString line;
        if(ret < 0) {
                line = QString("Slave threw exception  >  ").arg(ret) +  modbus_strerror(errno) + " ";
                qDebug()<<  "ModbusAdapter : modbusRequestData - " << line;
                rawModel->addLine(EUtils::SysTimeStamp() + " : " + line);
        }
        else {
                line = QString("Number of registers returned does not match number of registers requested!. [")  +  modbus_strerror(errno) + "]";
                qDebug()<<  "ModbusAdapter : modbusRequestData - " << "Number of registers returned does not match number of registers requested!";
                rawModel->addLine(EUtils::SysTimeStamp() + " : " + line);
        }
     }

}

void ModbusAdapter::busMonitorRequestData(uint8_t * data, uint8_t dataLen)
{

    //Request Raw data from port - Update raw data model

    QString line;

    for(int i = 0; i < dataLen; ++i ) {
        line += QString().sprintf( "%.2x  ", data[i] );
    }

    line = EUtils::TxTimeStamp(m_ModBusMode) + " : " + line;

    rawModel->addLine(line);

}
void ModbusAdapter::busMonitorResponseData(uint8_t * data, uint8_t dataLen)
{

    //Response Raw data from port - Update raw data model

    QString line;

    for(int i = 0; i < dataLen; ++i ) {
        line += QString().sprintf( "%.2x  ", data[i] );
    }

    line = EUtils::RxTimeStamp(m_ModBusMode) + " : " + line;

    rawModel->addLine(line);

}

extern "C" {

void busMonitorRawResponseData(uint8_t * data, uint8_t dataLen)
{
        m_instance->busMonitorResponseData(data, dataLen);
}

void busMonitorRawRequestData(uint8_t * data, uint8_t dataLen)
{
        m_instance->busMonitorRequestData(data, dataLen);
}

}
