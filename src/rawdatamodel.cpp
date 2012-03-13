#include "rawdatamodel.h"
#include <QtDebug>

RawDataModel::RawDataModel(QObject *parent) :
    QObject(parent)
{
    model = new  QStringListModel(this);
}

void RawDataModel::addLine(QString line)
{

    qWarning()<<  "RawDataModel : addLine - " << line << " , Lines = " << m_rawData.length();

    if (m_rawData.length() == m_maxNoOfLines)
        m_rawData.removeFirst();
    m_rawData.append(line);
    model->setStringList(m_rawData);

}

void RawDataModel::clear()
{

    qWarning()<<  "RawDataModel : clear" ;

    m_rawData.clear();
    model->setStringList(m_rawData);

}

void RawDataModel::setMaxNoOfLines(int noOfLines)
{
    m_maxNoOfLines = noOfLines;
}
