#include "registersmodel.h"
#include "QsLog.h"
#include <QStandardItem>
#include <QtDebug>

#include "eutils.h"

RegistersModel::RegistersModel(QObject *parent) :
    QObject(parent)
{
   model = new  QStandardItemModel(0,0,this);
   m_regDataDelegate = new RegistersDataDelegate(0);
   m_noOfItems = 0;
   m_is16Bit = false;
   m_isSigned = false;
   m_startAddrBase = 10;
   clear();
}

void RegistersModel::addItems(int startAddress, int noOfItems, bool valueIsEditable)
{
    int row;
    int col;
    m_startAddress = startAddress;
    m_noOfItems = noOfItems;

    QLOG_TRACE() <<  "Registers Model Address = " << startAddress << " , noOfItems = " << noOfItems
                << " , first row = " << m_firstRow << " , last row = " << m_lastRow;

    //Format Vertical - Horizontal Headers
    clear();
    if (noOfItems > 1) {
        if (m_frmt == EUtils::Float){
            m_firstRow = 0;
            m_lastRow = (noOfItems - 1) / 20;
            model->setHorizontalHeaderLabels(QStringList()<<RegModelFloatHeaderLabels[0]<<RegModelFloatHeaderLabels[1]
                                                        <<RegModelFloatHeaderLabels[2]<<RegModelFloatHeaderLabels[3]
                                                        <<RegModelFloatHeaderLabels[4]<<RegModelFloatHeaderLabels[5]
                                                        <<RegModelFloatHeaderLabels[6]<<RegModelFloatHeaderLabels[7]
                                                        <<RegModelFloatHeaderLabels[8]<<RegModelFloatHeaderLabels[9]);


            QStringList vertHeader;
            for (int i = m_firstRow; i <= m_lastRow; i++) {
                vertHeader<<QString("%1").arg(startAddress + i * 20, 2, 10, QLatin1Char('0'));
            }
            model->setVerticalHeaderLabels(vertHeader);
        }
        else {
            m_firstRow = 0;
            m_lastRow = (noOfItems - 1) / 10;
            model->setHorizontalHeaderLabels(QStringList()<<RegModelHeaderLabels[0]<<RegModelHeaderLabels[1]
                                                        <<RegModelHeaderLabels[2]<<RegModelHeaderLabels[3]
                                                        <<RegModelHeaderLabels[4]<<RegModelHeaderLabels[5]
                                                        <<RegModelHeaderLabels[6]<<RegModelHeaderLabels[7]
                                                        <<RegModelHeaderLabels[8]<<RegModelHeaderLabels[9]);

            QStringList vertHeader;
            for (int i = m_firstRow; i <= m_lastRow; i++) {
                vertHeader<<QString("%1").arg(startAddress + i * 10, 2, 10, QLatin1Char('0'));
            }
             model->setVerticalHeaderLabels(vertHeader);
        }
    }
    else {
        model->setHorizontalHeaderLabels(QStringList()<<RegModelHeaderLabels[0]);
        model->setVerticalHeaderLabels(QStringList()<<QString("%1").arg(startAddress, 2, 10, QLatin1Char('0')));
    }

    //Add data to model
    if (noOfItems == 1){
        QStandardItem *valueItem = new QStandardItem("-");model->setItem(0, 0, valueItem);
        valueItem->setEditable(valueIsEditable);
    }
    else {
        if (m_frmt == EUtils::Float){
            m_firstRow = 0;
            m_lastRow = (noOfItems - 1) / 20;
            for (int i = m_firstRow; i <= m_lastRow; i++) {
                row = i;
                for (int j = 0; j < 10; j++) {
                    col = j;
                    if ((row * 10 + col) >= (noOfItems / 2)){//not used cells
                        QStandardItem *valueItem = new QStandardItem("x");model->setItem(row, col, valueItem);
                        valueItem->setEditable(false);
                        valueItem->setForeground(QBrush(Qt::red));
                        valueItem->setBackground(QBrush(Qt::lightGray));
                    }
                    else {
                        QStandardItem *valueItem = new QStandardItem("-");model->setItem(row, col, valueItem);
                        valueItem->setEditable(valueIsEditable);
                    }
                }
            }
       }
        else {
            m_firstRow = 0;
            m_lastRow = (noOfItems - 1) / 10;
            for (int i = m_firstRow; i <= m_lastRow; i++) {
                row = i;
                for (int j = 0; j < 10; j++) {
                    col = j;
                    if ((row * 10 + col) >= (noOfItems)){//not used cells
                        QStandardItem *valueItem = new QStandardItem("x");model->setItem(row, col, valueItem);
                        valueItem->setEditable(false);
                        valueItem->setForeground(QBrush(Qt::red));
                        valueItem->setBackground(QBrush(Qt::lightGray));
                    }
                    else {
                        QStandardItem *valueItem = new QStandardItem("-");model->setItem(row, col, valueItem);
                        valueItem->setEditable(valueIsEditable);
                    }
                }
            }
        }
    }


    emit(refreshView());

}

void RegistersModel::setNoValidValues()
{

    int row;
    int col;
    //if we have no valid values we set  as value = '-/-'

    if (m_frmt == EUtils::Float){
        for (int i = 0; i < (m_noOfItems / 2); i++){
            row = i / 10;
            col = i % 10;
            QModelIndex index = model->index(row, col, QModelIndex());
            model->setData(index,QBrush(Qt::red),Qt::ForegroundRole);
            model->setData(index,"-/-",Qt::DisplayRole);
        }
    }
    else {
        for (int i = 0; i < m_noOfItems; i++){
            row = i / 10;
            col = i % 10;
            QModelIndex index = model->index(row, col, QModelIndex());
            model->setData(index,QBrush(Qt::red),Qt::ForegroundRole);
            model->setData(index,"-/-",Qt::DisplayRole);
        }
    }

}

void RegistersModel::setValue(int idx, int value)
{
    int row;
    int col;
    QString convertedValue;

    convertedValue = EUtils::formatValue(value, m_frmt, m_is16Bit, m_isSigned);

    //set model data
    if (m_noOfItems == 1){
        row = 0;
        col = 0;
    }
    else {
        row = (idx) / 10;
        col = (idx) % 10;
    }

    QModelIndex index = model->index(row, col, QModelIndex());
    model->setData(index,QBrush(Qt::black),Qt::ForegroundRole);
    model->setData(index,convertedValue,Qt::DisplayRole);
    model->setData(index,QString("Address : %1").arg(m_startAddress + idx, 1, m_startAddrBase).toUpper(),Qt::ToolTipRole);
}

void RegistersModel::setValue32(int idx, int valueHi, int valueLo)
{//TODO : update model with float values
    int row;
    int col;
    QString convertedValue;

    convertedValue = EUtils::formatValue32(valueHi, valueLo, m_endian, m_floatPrecision);

    //set model data
    if (m_noOfItems == 1){
        row = 0;
        col = 0;
    }
    else {
        row = (idx) / 10;
        col = (idx) % 10;
    }

    QModelIndex index = model->index(row, col, QModelIndex());
    model->setData(index,QBrush(Qt::black),Qt::ForegroundRole);
    model->setData(index,convertedValue,Qt::DisplayRole);
    model->setData(index,QString("Address : %1").arg(m_startAddress + 2*idx, 1, m_startAddrBase).toUpper(),Qt::ToolTipRole);
}

int RegistersModel::value(int idx)
{
    QString stringVal;
    int intVal;
    bool ok;

    //Get Value
    stringVal = strValue(idx);
    intVal = stringVal.toInt(&ok,m_frmt);
    if (ok)
        return intVal;
    else
        return -1;

}

float RegistersModel::floatValue(int idx)
{
    QString stringVal;
    float floatVal;
    bool ok;

    //Get Value
    stringVal = strValue(idx);
    floatVal = stringVal.toFloat(&ok);
    if (ok)
        return floatVal;
    else
        return -1;

}

QString RegistersModel::strValue(int idx)
{
    int row;
    int col;

    //get model data
    if (m_noOfItems == 1){
        row = 0;
        col = 0;
    }
    else {
        row = (idx) / 10;
        col = (idx) % 10;
    }
    QModelIndex index = model->index(row, col, QModelIndex());
    QVariant value = model->data(index,Qt::DisplayRole);
    if (value.canConvert<QString>())
        return value.toString();
    else
        return "-/-";

}

void RegistersModel::changeFrmt(int frmt)
{

    QString stringVal;
    int intVal;
    int row;
    int col;
    bool ok;
    QString convertedVal;

    QLOG_TRACE()<<  "Registers Model changed format from " << m_frmt << " to " << frmt ;

    //change base
    for (int idx = 0; idx < m_noOfItems ; idx++) {
        //Get Value
        stringVal = strValue(idx);
        intVal = stringVal.toInt(&ok,m_frmt);
        //Format Value
        if (ok)
            convertedVal = EUtils::formatValue(intVal, frmt, m_is16Bit, m_isSigned);
        else
            convertedVal = "-/-";
        //Update
        if (m_noOfItems == 1){
            row = 0;
            col = 0;
        }
        else {
            row = (idx) / 10;
            col = (idx) % 10;
        }
        QModelIndex index = model->index(row, col, QModelIndex());
        model->setData(index,convertedVal,Qt::DisplayRole);
        model->setData(index,QString("Address : %1").arg(m_startAddress + idx, 1, m_startAddrBase).toUpper(),Qt::ToolTipRole);
    }

    emit(refreshView());

}

void RegistersModel::clear()
{

    QLOG_TRACE()<<  "Registers Model Cleared" ;

    //Clear model
    model->clear();

}

void RegistersModel::setStartAddrBase(int base)
{

    QLOG_TRACE()<<  "Registers Model start addr set base = " << base ;

    m_startAddrBase = base;
    changeFrmt(m_frmt);

}

void RegistersModel::setFrmt(int frmt)
{

    QLOG_TRACE()<<  "Registers Model set base = " << frmt ;

    m_regDataDelegate->setFrmt(frmt);
    changeFrmt(frmt);
    m_frmt = frmt;

}

int RegistersModel::getFrmt()
{

    QLOG_TRACE()<<  "Registers Model get format = " << m_frmt ;

    return m_frmt;

}

void RegistersModel::setIs16Bit(bool is16Bit)
{

    QLOG_TRACE()<<  "Registers Model Is16Bit = " << is16Bit ;
    m_is16Bit = is16Bit;
    m_regDataDelegate->setIs16Bit(is16Bit);

}

void RegistersModel::setIsSigned(bool isSigned)
{

    QLOG_TRACE()<<  "Registers Model IsSigned = " << isSigned ;
    m_isSigned = isSigned;
    m_regDataDelegate->setIsSigned(isSigned);
    changeFrmt(m_frmt);

}

void RegistersModel::setEndian(int endian)
{

    QLOG_TRACE()<<  "Registers Model endianness = " << endian ;
    m_endian = endian;

}

int RegistersModel::getEndian()
{

    QLOG_TRACE()<<  "Registers Model endianness = " << m_endian ;
    return m_endian;

}

void RegistersModel::setFloatPrecision(int precision)
{

    QLOG_TRACE()<<  "Registers Model float precision = " << precision ;
    m_floatPrecision = precision;
    m_regDataDelegate->setFloatPrecision(precision);

}

RegistersDataDelegate* RegistersModel::itemDelegate()
{

    return m_regDataDelegate;

}
