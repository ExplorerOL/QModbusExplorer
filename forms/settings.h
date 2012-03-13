#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0 ,QSettings * settings = 0);
    ~Settings();

private:
    Ui::Settings *ui;
    QSettings * m_settings;

private slots:
    void changesAccepted();


protected:
    void showEvent(QShowEvent * event);

};

#endif // SETTINGS_H
