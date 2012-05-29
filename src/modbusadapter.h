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

     void modbusConnectRTU(QString port, int baud, QChar parity, int dataBits, int stopBits);
     void modbusConnectTCP(QString ip, int port);
     void modbusDisConnect();
     void modbusRequestData(int slave, int functionCode, int startAddress, int noOfItems);
     RegistersModel *regModel;
     RawDataModel *rawModel;
     bool isConnected();

private:
     modbus_t * m_modbus;
     bool m_connected;
     int m_ModBusMode;

signals:

public slots:


};

#endif // MODBUSADAPTER_H
