#include "contactlistmodel.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

int ContactListModel::groupToFetch = -1;

ContactListModel::ContactListModel(QObject *parent) : QAbstractItemModel(parent)
{
    connect(&DataProvider::getInstance(), SIGNAL(dataChunkReady(const dataChunkList&)), this, SLOT(handleDataReady(const dataChunkList&)));
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

void ContactListModel::handleDataReady(const dataChunkList& dataChunkList)
{
    foreach(DataChunkType pair, dataChunkList) {
        QVariantMap groupData = pair.first.toMap();
        int groupId = groupData.value("groupOrder").toInt();
        if (!mGroups.contains(groupId)) {
            mGroups[groupId] = new TreeItem(groupData, nullptr);
        }
        int start = mGroups.contains(groupId) ? mGroups[groupId]->row() : 0;
        beginInsertRows(index(groupId,0), start, start + pair.second.size());
        foreach(auto item, pair.second) {
            mGroups[groupId]->appendChild(new TreeItem(item.toMap().value("account"), mGroups[groupId]));
        }
        endInsertRows();
    }
}

bool ContactListModel::canFetchMore(const QModelIndex &parent) const
{
    //Note: Somehow when scrolling at the bottom the passed parent index is
    //invalid and not useful to identify the group, so applying a custom logic.
    Q_UNUSED(parent);
    groupToFetch = getGroupToFetch(); //Keep in static to not repeate the logic.
    return groupToFetch != -1;
}

void ContactListModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
    qDebug() <<"Fetching more for group " << groupToFetch;
    DataProvider::getInstance().fetchData(groupToFetch);
}

int ContactListModel::getGroupToFetch() const
{
    //we fetch data for a group which has last item visible and has more data to load.
    //when we get here there will be only one group satisfying the condition, assuming we can't have two big groups visible together
    int group = -1; //invlaid
    for (int i = 0; i < mGroups.size(); ++i) {
        QModelIndex indexToCheck = index(mGroups[i]->childCount()-1, 0, createIndex(i,0, mGroups[i]));
        if (mView->visualRect(indexToCheck).isValid() && DataProvider::getInstance().canFetch(i)) {
            group = i;
        }
    }
    return group;
}
