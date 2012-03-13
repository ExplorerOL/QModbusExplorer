#include "busmonitor.h"
#include "ui_busmonitor.h"
#include <QtDebug>
#include <QFile>
#include <QFileDialog>

BusMonitor::BusMonitor(QWidget *parent, RawDataModel *rawDataModel) :
    QMainWindow(parent),
    ui(new Ui::BusMonitor),
    m_rawDataModel(rawDataModel)
{
    ui->setupUi(this);
    ui->lstRawData->setModel(m_rawDataModel->model);

    //Setup Toolbar
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addAction(ui->actionClear);
    ui->toolBar->addAction(ui->actionExit);
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->actionClear,SIGNAL(triggered()),this,SLOT(clear()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exit()));

}

BusMonitor::~BusMonitor()
{
    delete ui;
}

void BusMonitor::save()
{

    qWarning()<<  "BusMonitor : save" ;

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

    qWarning()<<  "BusMonitor : clear" ;

    m_rawDataModel->clear();

}

void BusMonitor::exit()
{

    qWarning()<<  "BusMonitor : exit" ;

    this->close();

}
