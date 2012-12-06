#ifndef REGISTERSMODEL_H
#define REGISTERSMODEL_H

#include <QObject>
#include <QStandardItemModel>

static const QString RegModelHeaderLabels[]={"00", "01", "02", "03", "04", "05", "06", "07", "08", "09"};
static const int AddressColumn=0;
static const int ValueColumn=1;

class RegistersModel : public QObject
{
    Q_OBJECT
public:
    explicit RegistersModel(QObject *parent = 0);

    void addItems(int startAddress, int noOfItems, bool valueIsEditable);
    void setValue(int idx, int value);
    void setBase(int frmt);
    void setIs16Bit(bool is16Bit);
    QString strValue(int idx);
    int value(int idx);
    QStandardItemModel *model;
    void clear();
    void setNoValidValues();

private:
    void changeBase(int frmt);
    int m_startAddress;
    int m_noOfItems;
    int m_offset;
    int m_firstRow;
    int m_lastRow;
    bool m_is16Bit;
    int m_base;
    int m_frmt;

signals:
    void refreshView();

public slots:

};

#endif // REGISTERSMODEL_H
