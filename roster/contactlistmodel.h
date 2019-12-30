#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include <QJsonArray>

class ContactListModel : public QAbstractListModel
{
     Q_OBJECT
public:
    ContactListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & /*parent*/) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void handleDataReady(const QJsonDocument&);

private:
    QJsonArray mJsonArray;
};

#endif // CONTACTLISTMODEL_H
