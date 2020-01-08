#include "utils.h"
#include <QDebug>
#include <QPoint>

namespace utils {
void paintAvatar( QPainter& paint,
                  int size,
                  const QPoint& center,
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
    paint.drawEllipse(center, size/2, size/2);

    paint.setPen(textColor);
    QFont font = paint.font();
    font.setPointSizeF(size / 2.5);
    paint.setFont(font);

    QFontMetrics fm(font);
    int width = fm.width(text);
    int height = fm.height();
    paint.drawText(QPointF(center.x() - width / 2.0, center.y() + height / 4), text);
}

QString makeInitials(const QString& firstName, const QString& lastName)
{
    QString initials;
    initials.append(firstName.isEmpty() ? "" : QString(firstName.at(0)));
    initials.append(lastName.isEmpty() ? "" : QString(lastName.at(0)));
    if (initials.isEmpty()) {
       qWarning() << "Empty firstName and lastName for item";
       initials = "??";
    }
    return initials;
}

QColor getColorForSex(const QString& sex)
{
    static const QMap<QString, QColor> avatarColors = {{"MALE", QColor("#B5E6FF")}, {"FEMALE", QColor("#FCD0FC")}, {"NOT_DEFINED", QColor("#E1E8ED")}};
    return avatarColors.value(sex);
}

}
