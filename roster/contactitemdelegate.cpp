#include "contactitemdelegate.h"

#include <QPainter>
#include <QDebug>

const int itemHeight = 60;
const int textLeftPadding = 60;
const int avatarLeftPadding = 30;
const double PaintingScaleFactor = 1.5;
const QMap<QString, QColor> avatarColors = {{"MALE", QColor("#B5E6FF")}, {"FEMALE", QColor("#FCD0FC")}, {"NOT_DEFINED", QColor("#E1E8ED")}};

QString makeInitials(const QString& firstName, const QString& lastName);
void paintAvatar(QPainter&, int size, const QString& text, const QColor& backgroundColor, const QColor& textColor);

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
    QString initials = makeInitials(firstName, lastName);

    const double yOffset = (rect.height() - PaintingScaleFactor) / 2;
    painter->translate(rect.x(), rect.y() + yOffset);
    painter->scale(PaintingScaleFactor, PaintingScaleFactor);
    paintAvatar(*painter, 32, initials, avatarColors.value(itemData.value("sex").toString()), Qt::blue);
    //painter->drawText(0, 0, QString::number(index.row()) + ".");
    painter->drawText(textLeftPadding, 0,  firstName + " " + lastName);
    painter->restore();
}


QSize ContactItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(10,itemHeight);
}

void paintAvatar( QPainter& paint,
                  int size,
                  const QString& text,
                  const QColor& backgroundColor,
                  const QColor& textColor)
{
    if (size <= 0 || text.size() > 2 || text.size() == 0) {
        qCritical() << "Unexpected parameter for avatar. size: " << size << " text: " << text;
        return;
    }

    paint.setBrush(backgroundColor);
    paint.setPen(backgroundColor);
    paint.drawEllipse(QPoint(avatarLeftPadding , 0), size/2, size/2);

    paint.setPen(textColor);
    QFont font = paint.font();
    font.setPointSize(int(size / 2.2));
    paint.setFont(font);
    paint.drawText(QPoint(avatarLeftPadding-size/3, size/10), text);
}

QString makeInitials(const QString& firstName, const QString& lastName)
{
    QString initials;
    initials.append(firstName.isEmpty() ? QChar() : firstName.front());
    initials.append(lastName.isEmpty() ? QChar() : lastName.front());
    if (initials.isEmpty()) {
       qWarning() << "No firstName and lastName for item";
       initials = "??";
    }
    return initials;
}
