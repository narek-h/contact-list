#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include <QJsonArray>
#include "treeitem.h"
#include <QTreeView>
#include <dataprovider.h>

class ContactListModel : public QAbstractItemModel
{
     Q_OBJECT
public:
    ContactListModel(QObject *parent = nullptr);
    ~ContactListModel();
    int rowCount(const QModelIndex & /*parent*/) const;
    QVariant data(const QModelIndex &index, int role) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex	parent(const QModelIndex &index) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    void setTreeView(QTreeView* view) { mView = view; }

public slots:
    void handleDataReady(const dataChunkList&);
    void handleFilterTextChanged(const QString&);
signals:
    void fetchMoreSignal(int a = -1);

private:
    DataProvider* mDataProvider;
    int getGroupToFetch() const;
    //QJsonArray mJsonArray;
    QMap<int, TreeItem*> mGroups;
    QTreeView* mView; //Not good, but using as workaround to check the group for fetching data.
    static int groupToFetch;
    bool mFetchingData;
};

#endif // CONTACTLISTMODEL_H
