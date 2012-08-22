#include "about.h"
#include "ui_about.h"

const QString VER = "QModMaster 0.3.5";

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->lblVersion->setText(VER);
}

About::~About()
{
    delete ui;
}
