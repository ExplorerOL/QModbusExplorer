#ifndef BUSMONITOR_H
#define BUSMONITOR_H

#include <QMainWindow>
#include <QLabel>
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

protected:
    void closeEvent(QCloseEvent * event );

private slots:
    void clear();
    void exit();
    void save();
    void startStop(bool en);

};

#endif // BUSMONITOR_H
