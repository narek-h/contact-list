#include "contactlistmodel.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>

int ContactListModel::groupToFetch = -1;

ContactListModel::ContactListModel(QObject *parent) : QAbstractItemModel(parent), mView(nullptr), mFetchingData(false)
{
    mDataProvider = new DataProvider();
    //Move slots processing to different thread
    QThread* dataProcessingThread = new QThread(this);
    mDataProvider->moveToThread(dataProcessingThread);
    dataProcessingThread->start();
    qRegisterMetaType<dataChunkList>("dataChunkList");
    bool res = connect(mDataProvider, SIGNAL(dataChunkReady(const dataChunkList&)), this, SLOT(handleDataReady(const dataChunkList&)));
    Q_ASSERT(res);
    res = connect(dataProcessingThread, SIGNAL(finished()), mDataProvider, SLOT(deleteLater()));
    Q_ASSERT(res);
    res = connect(this, SIGNAL(destroyed()), dataProcessingThread, SLOT(quit()));
    Q_ASSERT(res);
    res = connect(dataProcessingThread, SIGNAL(finished()), dataProcessingThread, SLOT(deleteLater()));
    Q_ASSERT(res);

    res = connect(this, SIGNAL(fetchMoreSignal(int)), mDataProvider, SLOT(fetchData(int)));
    Q_ASSERT(res);

    mFetchingData = true;
    emit fetchMoreSignal();
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
    qDebug() <<"Received data chunk";
    foreach(DataChunkType pair, dataChunkList) {
        QVariantMap groupData = pair.first.toMap();
        int groupId = groupData.value("groupOrder").toInt();
        if (!mGroups.contains(groupId)) {
            beginInsertRows(QModelIndex(), 0, groupId);
            mGroups[groupId] = new TreeItem(groupData, nullptr);
            endInsertRows();
        }
        int start = mGroups.contains(groupId) ? mGroups[groupId]->row() : 0;
        beginInsertRows(index(groupId,0), start, start + pair.second.size());
        foreach(auto item, pair.second) {
            mGroups[groupId]->appendChild(new TreeItem(item.toMap().value("account"), mGroups[groupId]));
        }
        endInsertRows();
    }

    emit dataChanged(index(0,0, QModelIndex()), createIndex(mGroups.size() - 1, 0, mGroups.last()));
    if (mView) { //Need to repaint since on some cases the view is not getting updated properrly.
        mView->repaint();
    }
    mFetchingData = false;
}

bool ContactListModel::canFetchMore(const QModelIndex &parent) const
{
    if (mFetchingData) {
        return false;
    }

    //Note: Somehow when scrolling at the bottom the passed parent index is
    //invalid and not useful to identify the group, so applying a custom logic.
    Q_UNUSED(parent);
    groupToFetch = getGroupToFetch(); //Keep in static to not repeate the logic.
    return groupToFetch != -1;
}

void ContactListModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
    mFetchingData = true;
    emit fetchMoreSignal(groupToFetch);
}

int ContactListModel::getGroupToFetch() const
{
    if (!mView || mFetchingData) {
        return -1;
    }
    //we fetch data for a group which has last item visible and has more data to load.
    //when we get here there will be only one group satisfying the condition, assuming we can't have two big groups visible together
    int group = -1; //invlaid
    for (int i = 0; i < mGroups.size(); ++i) {
        QModelIndex parentIndex = createIndex(i, 0, mGroups[i]);
        QModelIndex indexToCheck = index(mGroups[i]->childCount()-1, 0, parentIndex);

        if (i > 0 && mView->isExpanded(createIndex(i - 1, 0, mGroups[i - 1])) && mView->visualRect(parentIndex).isValid()) {
            group = i - 1;
        } else if (mView->visualRect(indexToCheck).isValid() && mDataProvider->canFetch(i)) {
            group = i;
        }
    }
    return group;
}


void ContactListModel::handleFilterTextChanged(const QString& text)
{
    if (text.size() == 1) {
        return; //filter should be at least 2 symbols long
    }
    //Clean current list
    beginRemoveRows(QModelIndex(), 0, mGroups.size());
    qDeleteAll(mGroups);
    mGroups.clear();
    endRemoveRows();

    mDataProvider->setFilter(text); //TODO: Make syncronized?
    mFetchingData = true;
    emit fetchMoreSignal();
}
