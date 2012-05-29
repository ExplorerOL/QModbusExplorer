#include "rawdatadelegate.h"
#include <QtDebug>
#include <QPainter>

void RawDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{

    //TODO
    /*painter->save();
    if (qVariantCanConvert<QString>(index.data())) {
         QString val = qVariantValue<QString>(index.data());
         if (val.indexOf("Sys") > -1)
            painter->setPen(Qt::darkBlue);
         else if (val.indexOf("Tx") > -1)
            painter->setPen(Qt::darkGreen);
         else if (val.indexOf("Rx") > -1)
            painter->setPen(Qt::darkMagenta);
         else
            painter->setPen(Qt::black);

         painter->drawText(QRect(option.rect.left(),option.rect.top(),option.rect.width()
                                     ,option.rect.height()),val,option.displayAlignment);
         painter->setBackgroundMode(Qt::OpaqueMode);
         painter->setBackground(QBrush(Qt::red,Qt::Dense1Pattern));
    }
    else {
        painter->setPen(Qt::black);
        QStyledItemDelegate::paint(painter, option, index);
    }
    painter->restore();*/

    QStyledItemDelegate::paint(painter, option, index);

}
