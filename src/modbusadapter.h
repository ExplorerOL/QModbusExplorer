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

     int pollTime();
     void setPollTime(int pollTime);
     void startPolling();
     void stopPolling();
     bool pollingIsActive();
     void modbusConnectRTU(QString port, int baud, QChar parity, int dataBits, int stopBits);
     void modbusConnectTCP(QString ip, int port);
     void modbusDisConnect();
     void modbusRequestData(int slave, int functionCode, int startAddress, int noOfItems);
     RegistersModel *regModel;
     RawDataModel *rawModel;
     bool isConnected();

private:
     QTimer *m_pollTimer;
     int m_pollTime;
     modbus_t * m_modbus;
     bool m_connected;
     bool m_RTUSelected;

signals:

public slots:
     void pollForDataOnBus();

};

#endif // MODBUSADAPTER_H
