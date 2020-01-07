#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>

class TreeItem
{
public:
    explicit TreeItem(const QVariant &data, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data() const;
    int row() const;
    TreeItem *parentItem();

private:
    QVector<TreeItem*> m_childItems;
    QVariant m_itemData;
    TreeItem *m_parentItem;
};

#endif // TREEITEM_H
