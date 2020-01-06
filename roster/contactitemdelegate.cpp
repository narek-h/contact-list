#include "contactitemdelegate.h"
#include "utils.h"

#include <QPainter>
#include <QDebug>

const int itemHeight = 60;
const int textLeftPadding = 60;
const int avatarLeftPadding = 30;
const double PaintingScaleFactor = 1.5;

ContactItemDelegate::ContactItemDelegate(QWidget* parent):QStyledItemDelegate(parent)
{

}

void ContactItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    QMap<QString, QVariant> itemData = index.data().toMap();
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    painter->save();
    QRect rect = option.rect;
    //Item separators
    QPen pen;
    pen.setColor(QColor(Qt::lightGray));
    painter->setPen(pen);
    painter->drawRect(rect);

    //TODO: May be better to keep the model in Strings to not use toString()
    QString firstName = itemData.value("firstName").toString();
    QString lastName = itemData.value("lastName").toString();
    QString initials = utils::makeInitials(firstName, lastName);

    const double yOffset = (rect.height() - PaintingScaleFactor) / 2;
    painter->translate(rect.x(), rect.y() + yOffset);
    painter->scale(PaintingScaleFactor, PaintingScaleFactor);
    utils::paintAvatar(*painter, 32, QPoint(avatarLeftPadding , 0), initials, utils::getColorForSex(itemData.value("sex").toString()), Qt::blue);
    //painter->drawText(0, 0, QString::number(index.row()) + ".");
    painter->drawText(textLeftPadding, 0,  firstName + " " + lastName);
    painter->restore();
}


QSize ContactItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(10,itemHeight);
}
