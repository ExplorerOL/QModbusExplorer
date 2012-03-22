#include <QtDebug>
#include <QFile>
#include <QFileDialog>
#include <QCloseEvent>
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
    ui->lstRawData->setItemDelegate(new RawDataDelegate());
    //Setup Toolbar
    ui->toolBar->addAction(ui->actionStart_Stop);
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addAction(ui->actionClear);
    ui->toolBar->addAction(ui->actionExit);
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->actionClear,SIGNAL(triggered()),this,SLOT(clear()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exit()));
    connect(ui->actionStart_Stop,SIGNAL(toggled(bool)),this,SLOT(startStop(bool)));

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

    ui->actionStart_Stop->setChecked(false);
    this->close();

}

void BusMonitor::closeEvent( QCloseEvent * event )
{

    ui->actionStart_Stop->setChecked(false);
    event->accept();

}

void BusMonitor::startStop(bool en)
{

    m_rawDataModel->enableAddLines(en);

}
