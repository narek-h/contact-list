#ifndef UTILS_H
#define UTILS_H

#include <QPainter>
#include <QString>
#include <QMap>
#include <QColor>

namespace utils {

    void paintAvatar(QPainter&, int size, const QPoint& center, const QString& text, const QColor& backgroundColor, const QColor& textColor);
    QString makeInitials(const QString& firstName, const QString& lastName);
    QColor getColorForSex(const QString&);
}

#endif // UTILS_H
