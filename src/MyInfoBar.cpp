#include <QtWidgets>
#include "MyInfoBar.h"

void MyInfoBar::setType(enum MyInfoBarType type)
{
    switch (type) {
        case Question:
            this->setStyleSheet("background-color: skyblue");
            break;
        case Error:
            this->setStyleSheet("background-color: orange");
            break;
        default:
        case Warning:
            this->setStyleSheet("background-color: khaki");
            break;
    }
}

void MyInfoBar::setMessage(QString message)
{
    label->setText(message);
}

void MyInfoBar::showUp(QString message, enum MyInfoBarType type)
{
    this->setMessage(message);
    this->setType(type);
    this->show();
}

MyInfoBar::MyInfoBar(QWidget *parent)
{
    label = new QLabel;

    QPushButton *button = new QPushButton;
    button->setStyleSheet("QPushButton{image: url(:/img/close8-black.png);\
                                       border: none;\
                                       height: 12px;\
                                       width: 12px}\
                           QPushButton:hover{image: url(:/img/close8-red.png);\
                                             border: none;\
                                             height: 12px;\
                                             width: 12px}\
                           QPushButton:pressed{image: url(:/img/close8-red.png);\
                                             background-color: lightgrey;\
                                             border: 1px solid darkgrey;\
                                             border-radius: 2px;\
                                             height: 12px;\
                                             width: 12px}");
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    button->setFocusPolicy(Qt::NoFocus);
    connect(button, SIGNAL(clicked()), this, SLOT(hide()));

    hboxlayout = new QHBoxLayout;
    hboxlayout->addWidget(label);
    hboxlayout->addWidget(button);

    if (parent != NULL) {
        this->setParent(parent);
    }
    this->setLayout(hboxlayout);
    this->setFrameStyle(QFrame::Box);
    this->hide();
}

MyInfoBar::MyInfoBar(QString message, enum MyInfoBarType type, QWidget *parent) : MyInfoBar(parent)
{
    this->setType(type);
    this->setMessage(message);
}
