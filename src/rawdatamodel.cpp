#include "rawdatamodel.h"
#include "QsLog.h"
#include <QtDebug>

RawDataModel::RawDataModel(QObject *parent) :
    QObject(parent)
{
    model = new  QStringListModel(this);
    m_addLinesEnabled = false;
}

void RawDataModel::addLine(QString line)
{

    if ((!m_addLinesEnabled) || (line.length() == 0)) {
         QLOG_TRACE() <<  "Lines were not added";
        return;
    }

    QLOG_TRACE() <<  "Raw Data Model Line = " << line << " , No of lines = " << m_rawData.length();

    if (m_rawData.length() == m_maxNoOfLines)
        m_rawData.removeFirst();
    m_rawData.append(line);
    model->setStringList(m_rawData);

    if (m_rawData.length() !=0){
        emit newRawDataReceived();
    }

}

void RawDataModel::clear()
{

    QLOG_TRACE() <<  "Raw Data Model cleared" ;

    m_rawData.clear();
    model->setStringList(m_rawData);

}

void RawDataModel::setMaxNoOfLines(int noOfLines)
{
    m_maxNoOfLines = noOfLines;
}

void RawDataModel::enableAddLines(bool en)
{
    m_addLinesEnabled = en;
}
