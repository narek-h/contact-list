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
    font.setPointSize(int(size / 2.5));
    paint.setFont(font);
    paint.drawText(QPoint(center.x() - size/3, center.y()+ size/5), text);
}

QString makeInitials(const QString& firstName, const QString& lastName)
{
    QString initials;
    initials.append(firstName.isEmpty() ? QChar() : firstName.front());
    initials.append(lastName.isEmpty() ? QChar() : lastName.front());
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
