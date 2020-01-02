#include "contactitemdelegate.h"

#include <QPainter>
#include <QDebug>

const int itemHeight = 50;
const int textLeftPadding = 50;
const double PaintingScaleFactor = 1.5;

ContactItemDelegate::ContactItemDelegate(QWidget* parent):QStyledItemDelegate(parent)
{

}

void ContactItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{


    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    painter->save();
    QRect rect = option.rect;

    //Just to make the items more visible
    if (index.row() % 2 == 1) {
        QBrush br;
        br.setColor(QColor(Qt::lightGray));
        br.setStyle(Qt::SolidPattern);
        painter->fillRect(rect, br);
    }
    const double yOffset = (rect.height() - PaintingScaleFactor) / 2;
    painter->translate(rect.x(), rect.y() + yOffset);
    painter->scale(PaintingScaleFactor, PaintingScaleFactor);
    painter->drawText(0, 0, QString::number(index.row()) + ".");
    painter->drawText(textLeftPadding, 0, index.data().toString());
    painter->restore();
}



QSize ContactItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(10,itemHeight);
}
