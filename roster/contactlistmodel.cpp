#include "contactlistmodel.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "dataprovider.h"

ContactListModel::ContactListModel(QObject *parent) : QAbstractItemModel(parent)
{
    connect(&DataProvider::getInstance(), SIGNAL(dataReady(const QJsonDocument&)), this, SLOT(handleDataReady(QJsonDocument)));
    DataProvider::getInstance().fetchData();
}

ContactListModel::~ContactListModel()
{
    foreach(TreeItem* item, mGroups) {
        delete item;
    }
}
int ContactListModel::rowCount(const QModelIndex & parent) const
{
    //    return mJsonArray.size();
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
        return mGroups.size();
    } else {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
        return parentItem->childCount();
    }
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        QVariant data = item->data();
        if (!index.parent().isValid()) {
            data = item->data().toMap().value("group");
        }
        return data;
    }
    return QVariant();
}

int ContactListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return 1;
}


QModelIndex ContactListModel::index(int row, int column, const QModelIndex &parent) const
{
    TreeItem * parentItem;

    if ( !parent.isValid() )
        parentItem = nullptr;
    else
        parentItem = static_cast<TreeItem*>( parent.internalPointer() );

    TreeItem *childItem = parentItem ? parentItem->child( row ) : mGroups[row];
    if ( childItem )
        return createIndex( row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex	ContactListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == nullptr)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

void ContactListModel::handleDataReady(const QJsonDocument& json)
{
    QJsonArray jsonArray = json["roster"].toArray();
    qDebug() <<"check size" << jsonArray.size();

    //Add the items to tree structure
    foreach (QJsonValue item, jsonArray) {
        int groupId = item.toObject().value("groupOrder").toInt();
        if (!mGroups.contains(groupId)) {
            QVariantMap groupData;
            groupData["groupOrder"] = groupId;
            groupData["group"] = item.toObject().value("group");
            groupData["id"] = item.toObject().value("id");
            mGroups[groupId] = new TreeItem(groupData, nullptr);
        }
        mGroups[groupId]->appendChild(new TreeItem(item.toVariant().toMap().value("account"), mGroups[groupId]));
    }

    emit dataChanged(QModelIndex(), QModelIndex());
}
