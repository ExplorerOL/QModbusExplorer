#include "modbusadapter.h"
#include <QMessageBox>
#include <QtDebug>
#include <errno.h>

ModbusAdapter *m_instance;
static const int DBG = false; //Debug messages from libmodbus

ModbusAdapter::ModbusAdapter(QObject *parent) :
    QObject(parent),
    m_modbus(NULL)
{
    m_instance=this;
    m_pollTimer=new QTimer(this);
    //Do we need polling ?
    //m_pollTime=100;
    //connect(m_pollTimer, SIGNAL(timeout()), this, SLOT(pollForDataOnBus()));
    regModel=new RegistersModel(this);
    rawModel=new RawDataModel(this);
    m_connected = false;
}

void ModbusAdapter::modbusConnectRTU(QString port, int baud, QChar parity, int dataBits, int stopBits)
{
    //Modbus RTU connect

    qWarning()<<  "ModbusAdapter : modbusConnect RTU";

    modbusDisConnect();
    m_modbus = modbus_new_rtu(port.toAscii().constData(),baud,parity.toAscii(),dataBits,stopBits);

    //Debug messages from libmodbus
    modbus_set_debug(m_modbus, DBG);

    if(m_modbus && modbus_connect(m_modbus) == -1) {
        QMessageBox::critical(NULL, "Connection failed", "Could not connect to serial port!");
        m_connected = false;
    }
    else
        m_connected = true;

    m_RTUSelected = m_connected;

}

void ModbusAdapter::modbusConnectTCP(QString ip, int port)
{
    //Modbus RTU connect

    qWarning()<<  "ModbusAdapter : modbusConnect TCP";

    modbusDisConnect();
    m_modbus = modbus_new_tcp(ip.toAscii().constData(), port);

    //Debug messages from libmodbus
    modbus_set_debug(m_modbus, DBG);

    if(m_modbus && modbus_connect(m_modbus) == -1) {
        QMessageBox::critical(NULL, "Connection failed", "Could not connect to TCP port!");
        m_connected = false;
    }
    else
        m_connected = true;

    m_RTUSelected = false;

}


void ModbusAdapter::modbusDisConnect()
{
    //Modbus disconnect

    qWarning()<<  "ModbusAdapter : modbusDisConnect ";

    if(m_modbus) {
        modbus_close(m_modbus);
        modbus_free(m_modbus);
        m_modbus = NULL;
    }

    m_connected = false;

    m_RTUSelected = false;

}

bool ModbusAdapter::isConnected()
{
    //Modbus is connected

    return m_connected;
}

void ModbusAdapter::modbusRequestData(int slave, int functionCode, int startAddress, int noOfItems)
{
    //Modbus request data

    qWarning()<<  "ModbusAdapter : modbusRequestData ";

    if(m_modbus == NULL) return;

    uint8_t dest[1024]; //setup memory for data
    uint16_t * dest16 = (uint16_t *) dest;
    memset(dest, 0, 1024);
    int ret = -1; //return value from read functions
    bool is16Bit = false;
    bool writeAccess = false;

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
            case _FC_WRITE_SINGLE_COIL:
                    ret = modbus_write_bit(m_modbus, startAddress,regModel->value(0));
                    writeAccess = true;
                    noOfItems = 1;
                    break;
            case _FC_WRITE_SINGLE_REGISTER:
                    ret = modbus_write_register( m_modbus, startAddress,regModel->value(0));
                    writeAccess = true;
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
                    writeAccess = true;
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
                    writeAccess = true;
                    break;
            }

            default:
                    break;
    }

    qWarning()<<  "ModbusAdapter : modbusRequestData ret = " << ret;

    //update data model
    if(ret == noOfItems)
    {
            if(writeAccess)
            {
                //TODO - values writen correctly
            }
            else
            {
                for(int i = 0; i < noOfItems; ++i)
                {
                    int data = is16Bit ? dest16[i] : dest[i];
                    regModel->setValue(i,data);
                }
            }
    }
    else
    {

        regModel->setNoValidValues();

        QString line;
        if(ret < 0) {
                line = QString("Slave threw exception %1 or function not implemented. [").arg(ret) +  modbus_strerror(errno) + "]";
                qWarning()<<  "ModbusAdapter : modbusRequestData - " << line;
                rawModel->addLine(EUtils::SysTimeStamp() + " : " + line);
                //QMessageBox::critical(NULL,"Protocol error",QString("Slave threw exception %1 or function not implemented.").arg(ret));
        }
        else {
                line = QString("Number of registers returned does not match number of registers requested!. [")  +  modbus_strerror(errno) + "]";
                qWarning()<<  "ModbusAdapter : modbusRequestData - " << "Number of registers returned does not match number of registers requested!";
                rawModel->addLine(EUtils::SysTimeStamp() + " : " + line);
                //QMessageBox::critical(NULL,"Protocol error","Number of registers returned does not match number of registers requested!");
        }
     }


}

int ModbusAdapter::pollTime()
{
    //Poll time for bus monitoring

    return m_pollTime;
}

void ModbusAdapter::setPollTime(int pollTime)
{
 //Poll time for bus monitoring

    m_pollTime=pollTime;
}

void ModbusAdapter::startPolling()
{
    //start polling for bus monitoring

    m_pollTimer->start(m_pollTime);
}

void ModbusAdapter::stopPolling()
{
    //stop polling for bus monitoring

    m_pollTimer->stop();
}

bool ModbusAdapter::pollingIsActive()
{
    //polling for bus monitoring is active

    return m_pollTimer->isActive();
}

void ModbusAdapter::pollForDataOnBus()
{
    //polling for bus monitoring every PollTime msec

    //if(m_modbus)
    //    modbus_poll(m_modbus);
}

void ModbusAdapter::busMonitorRequestData(uint8_t * data, uint8_t dataLen)
{

    //Request Raw data from port - Update raw data model

    QString line;

    for(int i = 0; i < dataLen; ++i ) {
        //if we are using modbus TCP discharge TCP Header
        //if (i > 5 || m_RTUSelected) line += QString().sprintf( "%.2x  ", data[i] );
        line += QString().sprintf( "%.2x  ", data[i] );
    }

    line = EUtils::TxTimeStamp() + " : " + line;

    rawModel->addLine(line);

}
void ModbusAdapter::busMonitorResponseData(uint8_t * data, uint8_t dataLen)
{

    //Response Raw data from port - Update raw data model

    QString line;

    for(int i = 0; i < dataLen; ++i ) {
        //if we are using modbus TCP discharge TCP Header
        //if (i > 5 || m_RTUSelected) line += QString().sprintf( "%.2x  ", data[i] );
        line += QString().sprintf( "%.2x  ", data[i] );
    }

    line = EUtils::RxTimeStamp() + " : " + line;

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
