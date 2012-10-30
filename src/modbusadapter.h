#ifndef MODBUSADAPTER_H
#define MODBUSADAPTER_H

#include <QObject>
#include "modbus.h"
#include "registersmodel.h"
#include "rawdatamodel.h"
#include <QTimer>
#include "eutils.h"

class ModbusAdapter : public QObject
{
    Q_OBJECT
public:
    explicit ModbusAdapter(QObject *parent = 0);

     void busMonitorRequestData(uint8_t * data,uint8_t dataLen);
     void busMonitorResponseData(uint8_t * data,uint8_t dataLen);

     void modbusConnectRTU(QString port, int baud, QChar parity, int dataBits, int stopBits, int RTS, int timeOut=1);
     void modbusConnectTCP(QString ip, int port, int timeOut=1);
     void modbusDisConnect();
     void modbusTransaction(int slave, int functionCode, int startAddress, int noOfItems);
     RegistersModel *regModel;
     RawDataModel *rawModel;
     bool isConnected();

private:
     void modbusReadData(int slave, int functionCode, int startAddress, int noOfItems);
     void modbusWriteData(int slave, int functionCode, int startAddress, int noOfItems);
     modbus_t * m_modbus;
     bool m_connected;
     int m_ModBusMode;

signals:
    void refreshView();

public slots:


};

#endif // MODBUSADAPTER_H
