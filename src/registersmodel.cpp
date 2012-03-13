#include "registersmodel.h"
#include <QStandardItem>
#include <QtDebug>

RegistersModel::RegistersModel(QObject *parent) :
    QObject(parent)
{
   model = new  QStandardItemModel(0,2,this);
   m_noOfItems = 0;
   clear();
}

void RegistersModel::addItems(int startAddress, int noOfItems, bool valueIsEditable)
{

    qWarning()<<  "RegistersModel : address " << startAddress << " ,noOfItems " << noOfItems;

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
    qWarning()<<  "RegistersModel : setNoValidValues";

    for (int i = 0; i < m_noOfItems; i++){
        QModelIndex index = model->index(i, ValueColumn, QModelIndex());
        model->setData(index,"N/A",Qt::DisplayRole);
    }

}

void RegistersModel::setValue(int row, int value)
{
    QString convertedValue;

    convertedValue = QString("%1").arg(value,0,m_base).toUpper();

    qWarning()<<  "RegistersModel : setValue - row " << row <<" ,value " << convertedValue;

    //set model data
    QModelIndex index = model->index(row, ValueColumn, QModelIndex());
    model->setData(index,convertedValue,Qt::DisplayRole);

}

int RegistersModel::value(int row)
{
    //TODO Conversion not working

    qWarning()<<  "RegistersModel : value - row " << row;

    //get model data
    QModelIndex index = model->index(row, ValueColumn, QModelIndex());
    QVariant value = model->data(index,Qt::DisplayRole);
    if (value.canConvert<int>())
        return value.toInt();
    else
        return 0;

}

void RegistersModel::formatModel()
{
    //TODO Conversion not working

    int val;
    QString convertedVal;

    qWarning()<<  "RegistersModel : formatModel" ;

    //change base
    for (int row = 0; row < m_noOfItems ; row++) {
        //Get Value
        val = value(row);
        //Convert Value
        convertedVal = QString("%1").arg(val,0,m_base);
        //Update
        QModelIndex index = model->index(row, ValueColumn, QModelIndex());
        model->setData(index,convertedVal,Qt::DisplayRole);
    }


}

void RegistersModel::clear()
{

    qWarning()<<  "RegistersModel : clear" ;

    //Clear model
    model->clear();
    //Format
    model->setHorizontalHeaderLabels(QStringList()<<RegModelHeaderLabels[0]<<RegModelHeaderLabels[1]);

}

void RegistersModel::setBase(int base)
{

    qWarning()<<  "RegistersModel : setBase " << base ;
    m_base = base;

}
