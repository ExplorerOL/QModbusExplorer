#include "registersmodel.h"
#include <QStandardItem>
#include <QtDebug>

RegistersModel::RegistersModel(QObject *parent) :
    QObject(parent)
{
   model = new  QStandardItemModel(0,2,this);
   m_noOfItems = 0;
   m_is16Bit = false;
   clear();
}

void RegistersModel::addItems(int startAddress, int noOfItems, bool valueIsEditable)
{

    qDebug()<<  "RegistersModel : address " << startAddress << " ,noOfItems " << noOfItems;

    //m_dataType = dataType;
    m_startAddress = startAddress;
    m_noOfItems = noOfItems;

    //clear model
    clear();

    //Add data to model
    for (int row = 0; row < m_noOfItems ; row++) {
        //Address
        QStandardItem *addressItem = new QStandardItem(QString::number(m_startAddress+row));model->setItem(row, AddressColumn, addressItem);
        addressItem->setEditable(false);
        QStandardItem *valueItem = new QStandardItem("-");model->setItem(row, ValueColumn, valueItem);
        valueItem->setEditable(valueIsEditable);
    }

}

void RegistersModel::setNoValidValues()
{

    //if we have no valid values we set  as value = 'N/A'
    qDebug()<<  "RegistersModel : setNoValidValues";

    for (int i = 0; i < m_noOfItems; i++){
        QModelIndex index = model->index(i, ValueColumn, QModelIndex());
        model->setData(index,QBrush(Qt::red),Qt::ForegroundRole);
        model->setData(index,"N/A",Qt::DisplayRole);
    }

}

void RegistersModel::setValue(int row, int value)
{
    QString convertedValue;

    qDebug()<<  "RegistersModel : value - " << value << " is 16 Bit - " << m_is16Bit;

    convertedValue = formatValue(value, m_base, m_is16Bit);

    //set model data
    QModelIndex index = model->index(row, ValueColumn, QModelIndex());
    model->setData(index,QBrush(Qt::black),Qt::ForegroundRole);
    model->setData(index,convertedValue,Qt::DisplayRole);

}

QString RegistersModel::formatValue(int value,int base, bool is16Bit)
{
    QString convertedValue;

    qDebug()<<  "RegistersModel : formatValue value = " << value << ", base = " << base << ", is16Bit = " << is16Bit;


    switch(base){

        case 2://Binary
        if (is16Bit)
            convertedValue = QString("%1").arg(value,16,base,QLatin1Char('0')).toUpper();
        else
            convertedValue = QString("%1").arg(value,0,base).toUpper();
        break;

        case 10://Decimal
            convertedValue = QString("%1").arg(value,0,base).toUpper();
        break;

        case 16://Hex
        if (is16Bit)
            convertedValue = QString("%1").arg(value,4,base,QLatin1Char('0')).toUpper();
        else
            convertedValue = QString("%1").arg(value,0,base).toUpper();
        break;

        default://Default
        convertedValue = QString("%1").arg(value,0,base).toUpper();

    }

    return convertedValue;

}

int RegistersModel::value(int row)
{
    QString stringVal;
    int intVal;
    bool ok;

    qDebug()<<  "RegistersModel : value - row " << row;

    //Get Value
    stringVal = strValue(row);
    intVal = stringVal.toInt(&ok,m_base);
    if (ok)
        return intVal;
    else
        return -1;

}

QString RegistersModel::strValue(int row)
{

    qDebug()<<  "RegistersModel : strValue - row " << row;

    //get model data
    QModelIndex index = model->index(row, ValueColumn, QModelIndex());
    QVariant value = model->data(index,Qt::DisplayRole);
    if (value.canConvert<QString>())
        return value.toString();
    else
        return "N/A";

}

void RegistersModel::changeBase(int base)
{

    QString stringVal;
    int intVal;
    bool ok;
    QString convertedVal;

    qDebug()<<  "RegistersModel : changeBase from " << m_base << " to " << base ;

    //change base
    for (int row = 0; row < m_noOfItems ; row++) {
        //Get Value
        stringVal = strValue(row);
        intVal = stringVal.toInt(&ok,m_base);
        qDebug()<<  "RegistersModel : changeBase - stringVal = " << stringVal << " - intVal = " << intVal;
        //Format Value
        if (ok)
            convertedVal = formatValue(intVal, base, m_is16Bit);
        else
            convertedVal = "N/A";
        //Update
        QModelIndex index = model->index(row, ValueColumn, QModelIndex());
        model->setData(index,convertedVal,Qt::DisplayRole);
    }

}

void RegistersModel::clear()
{

    qDebug()<<  "RegistersModel : clear" ;

    //Clear model
    model->clear();
    //Format
    model->setHorizontalHeaderLabels(QStringList()<<RegModelHeaderLabels[0]<<RegModelHeaderLabels[1]);

}

void RegistersModel::setBase(int base)
{

    qDebug()<<  "RegistersModel : setBase " << base ;
    changeBase(base);
    m_base = base;

}

void RegistersModel::setIs16Bit(bool is16Bit)
{

    qDebug()<<  "RegistersModel : setIs16Bit " << is16Bit ;
    m_is16Bit = is16Bit;

}
