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
    void setValue(int row, int value);
    void setBase(int base);
    void setIs16Bit(bool is16Bit);
    QString strValue(int row);
    int value(int row);
    QStandardItemModel *model;
    void clear();
    void setNoValidValues();

private:

    void changeBase(int base);
    QString formatValue(int value,int base, bool is16Bit);
    int m_startAddress;
    int m_noOfItems;
    bool m_is16Bit;
    int m_base;

signals:

public slots:

};

#endif // REGISTERSMODEL_H
