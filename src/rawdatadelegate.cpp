#include "rawdatadelegate.h"
#include <QtDebug>
#include <QPainter>

void RawDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
     if (qVariantCanConvert<QString>(index.data())) {
         QString val = qVariantValue<QString>(index.data());
         //qWarning()<<  "RawDataDelegate data : " << val;
         if (val.indexOf("Sys")==0)
             painter->setPen(Qt::darkBlue);
         else if (val.indexOf("Tx")==0)
             painter->setPen(Qt::darkGreen);
         else if (val.indexOf("Rx")==0)
             painter->setPen(Qt::darkMagenta);
         else
             painter->setPen(Qt::black);
         painter->drawText(QRect(option.rect.left(),option.rect.top(),option.rect.width(),option.rect.height())
                           ,val,option.displayAlignment);
     }
     else {
         painter->setPen(Qt::black);
         QStyledItemDelegate::paint(painter, option, index);
     }
 }
