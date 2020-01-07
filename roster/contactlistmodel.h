#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include <QJsonArray>
#include "treeitem.h"

class ContactListModel : public QAbstractItemModel
{
     Q_OBJECT
public:
    ContactListModel(QObject *parent = nullptr);
    ~ContactListModel();
    int rowCount(const QModelIndex & /*parent*/) const;
    QVariant data(const QModelIndex &index, int role) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex	parent(const QModelIndex &index) const;

public slots:
    void handleDataReady(const QJsonDocument&);

private:
    //QJsonArray mJsonArray;
    QMap<int, TreeItem*> mGroups;
};

#endif // CONTACTLISTMODEL_H
