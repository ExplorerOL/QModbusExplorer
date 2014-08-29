#ifndef _MyInfoBar_H_
#define _MyInfoBar_H_

#include <QtWidgets>

class MyInfoBar : public QFrame
{
    Q_OBJECT

    public:

        enum MyInfoBarType {
            Question,
            Warning,
            Error
        };

        MyInfoBar(QWidget *parent = 0);
        MyInfoBar(QString message, enum MyInfoBarType type, QWidget *parent = 0);
        QLabel *label;
        QHBoxLayout *hboxlayout;
        void setType(enum MyInfoBarType type);
        void setMessage(QString string);
        void showUp(QString message, enum MyInfoBarType type);
};

#endif
