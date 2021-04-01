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
    //TODO : delegate
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
    if (selected.data().canConvert(QVariant::String)) {
            QString val = selected.data().value<QString>();
            qDebug()<<  "BusMonitor : selectedRow - " << val;
            if (val.indexOf("Sys") > -1)
                parseSysMsg(val);
            else if (val.indexOf("Tx") > -1)
                parseTxMsg(val);
            else if (val.indexOf("Rx") > -1)
                parseRxMsg(val);
            else
                ui->txtADU1->setPlainText("Type : Unknown Message");

    }
}

void BusMonitor::parseTxMsg(QString msg)
{
    ui->txtADU1->setPlainText("Type : Tx Message");
    QStringList row = msg.split(QRegExp("\\s+"));
        ui->txtADU1->appendPlainText("Timestamp : " + row[2]);
        if (msg.indexOf("RTU") > -1){//RTU message
            QStringList pdu;
            if (row.length() < 5){//check message length
                ui->txtADU1->appendPlainText("Error! Cannot parse Message");
                return;
            }
            for (int i = 4; i < row.length() - 1 ; i++)
                pdu.append(row[i]);
            parseTxPDU(pdu, "Slave Addr : ");
            ui->txtADU1->appendPlainText("CRC : " + pdu[pdu.length() - 2] + pdu[pdu.length() - 1]);
        }
        else if (msg.indexOf("TCP") > -1){//TCP message
            if (row.length() < 11){//check message length
                ui->txtADU1->appendPlainText("Error! Cannot parse Message");
                return;
            }
            ui->txtADU1->appendPlainText("Transaction ID : " + row[4] + row[5]);
            ui->txtADU1->appendPlainText("Protocol ID : " + row[6] + row[7]);
            ui->txtADU1->appendPlainText("Length : " + row[8] + row[9]);
            QStringList pdu;
            for (int i = 10; i < row.length() - 1 ; i++)
                pdu.append(row[i]);
            parseTxPDU(pdu, "Unit ID : ");
        }
        else
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");

}

void BusMonitor::parseTxPDU(QStringList pdu, QString slave)
{

    if (pdu.length() < 6){//check message length
        ui->txtADU1->appendPlainText(slave + pdu[0]);
        ui->txtADU1->appendPlainText("Function Code : " + pdu[1]);
        //ui->txtADU1->appendPlainText("Error! Cannot parse Message");
        return;
    }
    ui->txtADU1->appendPlainText(slave + pdu[0]);
    ui->txtADU1->appendPlainText("Function Code : " + pdu[1]);
    ui->txtADU1->appendPlainText("Starting Address : " + pdu[2] + pdu[3]);
    bool ok;
    int fcode = pdu[1].toInt(&ok,16);
    if (fcode == 1 || fcode == 2 || fcode == 3 || fcode == 4){//read
        ui->txtADU1->appendPlainText("Quantity of Registers : " + pdu[4] + pdu[5]);
    }
    else if (fcode == 5 || fcode == 6){//write
        ui->txtADU1->appendPlainText("Output Value : " + pdu[4] + pdu[5]);
    }
    else if (fcode == 15 || fcode == 16){//write multiple
        ui->txtADU1->appendPlainText("Quantity of Registers : " + pdu[4] + pdu[5]);
        if (pdu.length() < 8){//check message length
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");
            return;
        }
        ui->txtADU1->appendPlainText("Byte Count : " + pdu[6]);
        int byteCount = pdu[6].toInt(&ok,16);
        QString outputValues = "";
        for (int i = 7; i < 7 + byteCount; i++)
            outputValues += pdu[i] + " ";
        ui->txtADU1->appendPlainText("Output Values : " + outputValues);
    }

}

void BusMonitor::parseRxMsg(QString msg)
{
    ui->txtADU1->setPlainText("Type : Rx Message");
    QStringList row = msg.split(QRegExp("\\s+"));
    ui->txtADU1->appendPlainText("Timestamp : " + row[2]);
    if (msg.indexOf("RTU") > -1){//RTU message
        QStringList pdu;
        if (row.length() < 5){//check message length
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");
            return;
        }
        for (int i = 4; i < row.length() - 1 ; i++)
            pdu.append(row[i]);
        parseRxPDU(pdu, "Slave Addr : ");
        ui->txtADU1->appendPlainText("CRC : " + pdu[pdu.length() - 2] + pdu[pdu.length() - 1]);
    }
    else if (msg.indexOf("TCP") > -1){//TCP message
        if (row.length() < 11){//check message length
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");
            return;
        }
        ui->txtADU1->appendPlainText("Transaction ID : " + row[4] + row[5]);
        ui->txtADU1->appendPlainText("Protocol ID : " + row[6] + row[7]);
        ui->txtADU1->appendPlainText("Length : " + row[8] + row[9]);
        QStringList pdu;
        for (int i = 10; i < row.length() - 1 ; i++)
            pdu.append(row[i]);
        parseRxPDU(pdu, "Unit ID : ");
    }
    else
        ui->txtADU1->appendPlainText("Error! Cannot parse Message");

}

void BusMonitor::parseRxPDU(QStringList pdu, QString slave)
{

    bool ok;
    int fcode = pdu[1].toInt(&ok,16);
    if (fcode == 1 || fcode == 2 || fcode == 3 || fcode == 4){//read
        if (pdu.length() < 4){//check message length
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");
            return;
        }
        ui->txtADU1->appendPlainText(slave + pdu[0]);
        ui->txtADU1->appendPlainText("Function Code : " + pdu[1]);
        ui->txtADU1->appendPlainText("Byte Count : " + pdu[2]);
        int byteCount = pdu[2].toInt(&ok,16);
        QString inputValues = "";
        for (int i = 3; i < 3 + byteCount; i++)
            inputValues += pdu[i] + " ";
        ui->txtADU1->appendPlainText("Register Values : " + inputValues);
    }
    else if (fcode == 5 || fcode == 6){//write
        if (pdu.length() < 6){//check message length
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");
            return;
        }
        ui->txtADU1->appendPlainText(slave + pdu[0]);
        ui->txtADU1->appendPlainText("Function Code : " + pdu[1]);
        ui->txtADU1->appendPlainText("Starting Address : " + pdu[2] + pdu[3]);
        ui->txtADU1->appendPlainText("Output Value : " + pdu[4] + pdu[5]);
    }
    else if (fcode == 15 || fcode == 16){//write multiple
        if (pdu.length() < 6){//check message length
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");
            return;
        }
        ui->txtADU1->appendPlainText(slave + pdu[0]);
        ui->txtADU1->appendPlainText("Function Code : " + pdu[1]);
        ui->txtADU1->appendPlainText("Starting Address : " + pdu[2] + pdu[3]);
        ui->txtADU1->appendPlainText("Quantity of Registers : " + pdu[4] + pdu[5]);
    }
    else if (fcode > 0x80){//exception
        if (pdu.length() < 3){//check message length
            ui->txtADU1->appendPlainText("Error! Cannot parse Message");
            return;
        }
        ui->txtADU1->appendPlainText(slave + pdu[0]);
        ui->txtADU1->appendPlainText("Function Code [80 + Rx Function Code] : " + pdu[1]);
        ui->txtADU1->appendPlainText("Exception Code : " + pdu[2]);
    }

}

void BusMonitor::parseSysMsg(QString msg)
{
    ui->txtADU1->setPlainText("Type : System Message");
    QStringList row = msg.split(QRegExp("\\s+"));
    ui->txtADU1->appendPlainText("Timestamp : " + row[2]);
    ui->txtADU1->appendPlainText("Message" + msg.mid(msg.indexOf(" : ")));
}
