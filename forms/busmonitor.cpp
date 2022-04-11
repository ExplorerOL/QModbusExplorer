#include <QtDebug>
#include <QFile>
#include <QFileDialog>
#include <QCloseEvent>
#include <QShowEvent>
#include "busmonitor.h"
#include "ui_busmonitor.h"
#include "./src/rawdatadelegate.h"


BusMonitor::BusMonitor(QWidget *parent, RawDataModel *rawDataModel) :
    QMainWindow(parent),
    ui(new Ui::BusMonitor),
    m_rawDataModel(rawDataModel)
{
    ui->setupUi(this);
    ui->lstRawData->setModel(m_rawDataModel->model);
    //TODO : use delegate
    //ui->lstRawData->setItemDelegate(new RawDataDelegate());
    //Setup Toolbar
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addAction(ui->actionClear);
    ui->toolBar->addAction(ui->actionSxS);
    ui->toolBar->addAction(ui->actionExit);
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->actionClear,SIGNAL(triggered()),this,SLOT(clear()));
    connect(ui->actionSxS,SIGNAL(triggered()),this,SLOT(SxS()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exit()));
    connect(ui->lstRawData,SIGNAL(activated(QModelIndex)),this,SLOT(selectedRow(QModelIndex)));
    connect(ui->lstRawData,SIGNAL(clicked(QModelIndex)),this,SLOT(selectedRow(QModelIndex)));
    ui->lblADU2->setVisible(false);
    ui->txtADU2->setVisible(false);

    //connect(ui->lstRawData,SIGNAL(indexesMoved()),this,SLOT(scrollRawData()));
    //ui->lstRawData->scrollToBottom();
    connect(m_rawDataModel,SIGNAL(newRawDataReceived()),this,SLOT(updateBusRawData()));


}

BusMonitor::~BusMonitor()
{
    delete ui;
}

void BusMonitor::save()
{


    //Select file
    QString fileName = QFileDialog::getSaveFileName(NULL,"Save File As...",
                                                    QDir::homePath(),"Text (*.txt)",0,
                                                    QFileDialog::DontConfirmOverwrite);

    //Open File
    if (fileName.isEmpty())
        return;
    //continue only if a file name exists
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    //Text Stream
    QTextStream ts(&file);
    QStringList sl = m_rawDataModel->model->stringList();

    //iterate
    for (int i = 0; i < sl.size(); ++i)
              ts << sl.at(i) << endl;

    //Close File
    file.close();

}

void BusMonitor::clear()
{

    //qDebug()<<  "BusMonitor : clear" ;

    m_rawDataModel->clear();
    ui->txtADU1->clear();

}

void BusMonitor::SxS()
{

    //qDebug()<<  "BusMonitor : SxS" ;
    if (ui->actionSxS->isChecked()) {
        ui->lblADU1->setText("Tx ADU");
        ui->lblADU2->setVisible(true);
        ui->txtADU2->setVisible(true);
    }
    else {
        ui->lblADU1->setText("ADU");
        ui->lblADU2->setVisible(false);
        ui->txtADU2->setVisible(false);
    }

    ui->txtADU1->setPlainText("");
    ui->txtADU2->setPlainText("");

}

void BusMonitor::exit()
{

   //qDebug()<<  "BusMonitor : exit" ;

   this->close();

}

void BusMonitor::closeEvent(QCloseEvent *event)
{

    m_rawDataModel->enableAddLines(false);
    clear();
    event->accept();

}

void BusMonitor::showEvent(QShowEvent *event)
{

    m_rawDataModel->enableAddLines(true);
    event->accept();

}

void BusMonitor::selectedRow(const QModelIndex & selected)
{
int rowCount;
int currRow;
QModelIndex next;
QModelIndex prev;


    rowCount = ui->lstRawData->model()->rowCount();
    currRow = selected.row();
    if (currRow < rowCount - 1){
        next = ui->lstRawData->model()->index(currRow +1, 0);
        qDebug()<<  "Next Row : "<< next.row();
        qDebug()<<  "Next Data : "<< next.data();
    }


    if (ui->actionSxS->isChecked()) { //Side by Side Tx - Rx packests
        if (selected.data().canConvert(QVariant::String)) {
                QString val = selected.data().value<QString>();
                qDebug()<<  "BusMonitor : selectedRow - " << val;
                if (val.indexOf("Sys") > -1)
                    parseSysMsg(val, ui->txtADU1);
                else if (val.indexOf("Tx") > -1){
                    parseTxMsg(val, ui->txtADU1);
                   if (currRow < rowCount - 1){
                        next = ui->lstRawData->model()->index(currRow + 1, 0);
                        QString valNext = next.data().value<QString>();
                        if (valNext.indexOf("Rx") > -1)
                           parseRxMsg(valNext, ui->txtADU2);
                   }
                   else {
                        ui->txtADU2->setPlainText("-");
                   }
                }
                else if (val.indexOf("Rx") > -1){
                    parseRxMsg(val, ui->txtADU2);
                    if (currRow > 0){
                         prev = ui->lstRawData->model()->index(currRow - 1, 0);
                         QString valPrev = prev.data().value<QString>();
                         if (valPrev.indexOf("Tx") > -1)
                            parseTxMsg(valPrev, ui->txtADU1);
                    }
                    else {
                         ui->txtADU1->setPlainText("-");
                    }
                }
                else
                    ui->txtADU1->setPlainText("Type : Unknown Message");
        }
    }
    else {
        if (selected.data().canConvert(QVariant::String)) {
                QString val = selected.data().value<QString>();
                qDebug()<<  "BusMonitor : selectedRow - " << val;
                if (val.indexOf("Sys") > -1)
                    parseSysMsg(val, ui->txtADU1);
                else if (val.indexOf("Tx") > -1)
                    parseTxMsg(val, ui->txtADU1);
                else if (val.indexOf("Rx") > -1)
                    parseRxMsg(val, ui->txtADU1);
                else
                    ui->txtADU1->setPlainText("Type : Unknown Message");
        }
    }
}

void BusMonitor::parseTxMsg(QString msg, QPlainTextEdit* txtADU)
{
    txtADU->setPlainText("Type : Tx Message");
    QStringList row = msg.split(QRegExp("\\s+"));
        txtADU->appendPlainText("Timestamp : " + row[2]);
        if (msg.indexOf("RTU") > -1){//RTU message
            QStringList pdu;
            if (row.length() < 5){//check message length
                txtADU->appendPlainText("Error! Cannot parse Message");
                return;
            }
            for (int i = 4; i < row.length() - 1 ; i++)
                pdu.append(row[i]);
            parseTxPDU(pdu, "Slave Addr : ", txtADU);
            txtADU->appendPlainText("CRC : " + pdu[pdu.length() - 2] + pdu[pdu.length() - 1]);
        }
        else if (msg.indexOf("TCP") > -1){//TCP message
            if (row.length() < 11){//check message length
                txtADU->appendPlainText("Error! Cannot parse Message");
                return;
            }
            txtADU->appendPlainText("Transaction ID : " + row[4] + row[5]);
            txtADU->appendPlainText("Protocol ID : " + row[6] + row[7]);
            txtADU->appendPlainText("Length : " + row[8] + row[9]);
            QStringList pdu;
            for (int i = 10; i < row.length() - 1 ; i++)
                pdu.append(row[i]);
            parseTxPDU(pdu, "Unit ID : ", txtADU);
        }
        else
            txtADU->appendPlainText("Error! Cannot parse Message");

}

void BusMonitor::parseTxPDU(QStringList pdu, QString slave, QPlainTextEdit* txtADU)
{

    if (pdu.length() < 6){//check message length
        txtADU->appendPlainText(slave + pdu[0]);
        txtADU->appendPlainText("Function Code : " + pdu[1]);
        //txtADU->appendPlainText("Error! Cannot parse Message");
        return;
    }
    txtADU->appendPlainText(slave + pdu[0]);
    txtADU->appendPlainText("Function Code : " + pdu[1]);
    txtADU->appendPlainText("Starting Address : " + pdu[2] + pdu[3]);
    bool ok;
    int fcode = pdu[1].toInt(&ok,16);
    if (fcode == 1 || fcode == 2 || fcode == 3 || fcode == 4){//read
        txtADU->appendPlainText("Quantity of Registers : " + pdu[4] + pdu[5]);
    }
    else if (fcode == 5 || fcode == 6){//write
        txtADU->appendPlainText("Output Value : " + pdu[4] + pdu[5]);
    }
    else if (fcode == 15 || fcode == 16){//write multiple
        txtADU->appendPlainText("Quantity of Registers : " + pdu[4] + pdu[5]);
        if (pdu.length() < 8){//check message length
            txtADU->appendPlainText("Error! Cannot parse Message");
            return;
        }
        txtADU->appendPlainText("Byte Count : " + pdu[6]);
        int byteCount = pdu[6].toInt(&ok,16);
        QString outputValues = "";
        for (int i = 7; i < 7 + byteCount; i++)
            outputValues += pdu[i] + " ";
        txtADU->appendPlainText("Output Values : " + outputValues);
    }

}

void BusMonitor::parseRxMsg(QString msg, QPlainTextEdit* txtADU)
{
    txtADU->setPlainText("Type : Rx Message");
    QStringList row = msg.split(QRegExp("\\s+"));
    txtADU->appendPlainText("Timestamp : " + row[2]);
    if (msg.indexOf("RTU") > -1){//RTU message
        QStringList pdu;
        if (row.length() < 5){//check message length
            txtADU->appendPlainText("Error! Cannot parse Message");
            return;
        }
        for (int i = 4; i < row.length() - 1 ; i++)
            pdu.append(row[i]);
        parseRxPDU(pdu, "Slave Addr : ", txtADU);
        txtADU->appendPlainText("CRC : " + pdu[pdu.length() - 2] + pdu[pdu.length() - 1]);
    }
    else if (msg.indexOf("TCP") > -1){//TCP message
        if (row.length() < 11){//check message length
            txtADU->appendPlainText("Error! Cannot parse Message");
            return;
        }
        txtADU->appendPlainText("Transaction ID : " + row[4] + row[5]);
        txtADU->appendPlainText("Protocol ID : " + row[6] + row[7]);
        txtADU->appendPlainText("Length : " + row[8] + row[9]);
        QStringList pdu;
        for (int i = 10; i < row.length() - 1 ; i++)
            pdu.append(row[i]);
        parseRxPDU(pdu, "Unit ID : ", txtADU);
    }
    else
        txtADU->appendPlainText("Error! Cannot parse Message");

}

void BusMonitor::parseRxPDU(QStringList pdu, QString slave, QPlainTextEdit* txtADU)
{

    bool ok;
    int fcode = pdu[1].toInt(&ok,16);
    if (fcode == 1 || fcode == 2 || fcode == 3 || fcode == 4){//read
        if (pdu.length() < 4){//check message length
            txtADU->appendPlainText("Error! Cannot parse Message");
            return;
        }
        txtADU->appendPlainText(slave + pdu[0]);
        txtADU->appendPlainText("Function Code : " + pdu[1]);
        txtADU->appendPlainText("Byte Count : " + pdu[2]);
        int byteCount = pdu[2].toInt(&ok,16);
        QString inputValues = "";
        for (int i = 3; i < 3 + byteCount; i++)
            inputValues += pdu[i] + " ";
        txtADU->appendPlainText("Register Values : " + inputValues);
    }
    else if (fcode == 5 || fcode == 6){//write
        if (pdu.length() < 6){//check message length
            txtADU->appendPlainText("Error! Cannot parse Message");
            return;
        }
        txtADU->appendPlainText(slave + pdu[0]);
        txtADU->appendPlainText("Function Code : " + pdu[1]);
        txtADU->appendPlainText("Starting Address : " + pdu[2] + pdu[3]);
        txtADU->appendPlainText("Output Value : " + pdu[4] + pdu[5]);
    }
    else if (fcode == 15 || fcode == 16){//write multiple
        if (pdu.length() < 6){//check message length
            txtADU->appendPlainText("Error! Cannot parse Message");
            return;
        }
        txtADU->appendPlainText(slave + pdu[0]);
        txtADU->appendPlainText("Function Code : " + pdu[1]);
        txtADU->appendPlainText("Starting Address : " + pdu[2] + pdu[3]);
        txtADU->appendPlainText("Quantity of Registers : " + pdu[4] + pdu[5]);
    }
    else if (fcode > 0x80){//exception
        if (pdu.length() < 3){//check message length
            txtADU->appendPlainText("Error! Cannot parse Message");
            return;
        }
        txtADU->appendPlainText(slave + pdu[0]);
        txtADU->appendPlainText("Function Code [80 + Rx Function Code] : " + pdu[1]);
        txtADU->appendPlainText("Exception Code : " + pdu[2]);
    }

}

void BusMonitor::parseSysMsg(QString msg, QPlainTextEdit* txtADU)
{
    txtADU->setPlainText("Type : System Message");
    QStringList row = msg.split(QRegExp("\\s+"));
    txtADU->appendPlainText("Timestamp : " + row[2]);
    txtADU->appendPlainText("Message" + msg.mid(msg.indexOf(" : ")));
}

void BusMonitor::updateBusRawData(){
    ui->lstRawData->scrollToBottom();
}
