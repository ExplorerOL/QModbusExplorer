#ifndef BUSMONITOR_H
#define BUSMONITOR_H

#include <QMainWindow>
#include <QLabel>
#include <QPlainTextEdit>
#include "src/rawdatamodel.h"

namespace Ui {
    class BusMonitor;
}

class BusMonitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit BusMonitor(QWidget *parent, RawDataModel *rawDataModel);
    ~BusMonitor();

private:
    Ui::BusMonitor *ui;
    RawDataModel *m_rawDataModel;
    void parseTxMsg(QString msg, QPlainTextEdit* txtADU);
    void parseTxPDU(QStringList pdu, QString slave, QPlainTextEdit* txtADU);
    void parseRxMsg(QString msg, QPlainTextEdit* txtADU);
    void parseRxPDU(QStringList pdu, QString slave, QPlainTextEdit* txtADU);
    void parseSysMsg(QString msg, QPlainTextEdit* txtADU);

protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void clear();
    void exit();
    void save();
    void SxS();
    void selectedRow(const QModelIndex & selected);

};

#endif // BUSMONITOR_H
