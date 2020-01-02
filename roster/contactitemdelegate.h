#ifndef CONTACTITEMDELEGATE_H
#define CONTACTITEMDELEGATE_H

#include <QStyledItemDelegate>

class ContactItemDelegate : public QStyledItemDelegate
{
public:
    ContactItemDelegate(QWidget* parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CONTACTITEMDELEGATE_H
