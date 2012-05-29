#ifndef REGISTERSMODEL_H
#define REGISTERSMODEL_H

#include <QObject>
#include <QStandardItemModel>

static const QString RegModelHeaderLabels[]={"Address","Value"};
static const int AddressColumn=0;
static const int ValueColumn=1;

class RegistersModel : public QObject
{
    Q_OBJECT
public:
    explicit RegistersModel(QObject *parent = 0);

    void addItems(int startAddress, int noOfItems, bool valueIsEditable);
    void setValue(int row, int value, bool is16Bit);
    void setBase(int base);
    int value(int row);
    QStandardItemModel *model;
    void clear();
    void setNoValidValues();

private:

    void formatModel();
    int m_startAddress;
    int m_noOfItems;
    int m_base;

signals:

public slots:

};

#endif // REGISTERSMODEL_H
