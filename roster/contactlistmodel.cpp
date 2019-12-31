#include "contactlistmodel.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "dataprovider.h"

ContactListModel::ContactListModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(&DataProvider::getInstance(), SIGNAL(dataReady(const QJsonDocument&)), this, SLOT(handleDataReady(QJsonDocument)));
    DataProvider::getInstance().fetchData();
}

int ContactListModel::rowCount(const QModelIndex &) const
{
    return mJsonArray.size();
}


QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        QJsonObject obj = mJsonArray[index.row()].toObject()["account"].toObject();
        return obj["firstName"].toString() + "  " + obj["lastName"].toString();
    }
    return QVariant();
}

void ContactListModel::handleDataReady(const QJsonDocument& json) {
    mJsonArray = json["roster"].toArray();
    qDebug() <<"check size" << mJsonArray.size();
    emit dataChanged(QModelIndex(), QModelIndex());
}
