#include "dataprovider.h"
#include "downloader.h"

#include <QStandardPaths>
#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

const QString filePath = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0] + "/data.json";
const QVector<int> groupOrders = {0, 1, 2}; //Hardcoding the list for simplicity but the code can be adjusted to extract this from the data.
const int dataChunkSize = 100;

DataProvider::DataProvider(QObject *parent) : QObject(parent)
{
}

DataProvider& DataProvider::getInstance()
{
    static DataProvider mInstance;
    return mInstance;
}

void DataProvider::fetchData(int group)
{
    QFile dataFile(filePath);
    if (!dataFile.exists()) {
        Downloader * down = new Downloader(this);
        down->downloadDataFile();
        connect(down, SIGNAL(downloadFinished(const QByteArray&)), this, SLOT(handleDownloadFinished(const QByteArray&)));
    } else if (dataFile.open(QFile::ReadOnly))  {
        QJsonDocument data = QJsonDocument::fromJson(dataFile.readAll());
        splitJsonToGroups(data); //do it once
        dataChunkList listToSend;
        foreach(int gr, groupOrders) {
            if (mItemsByGroups.contains(gr)) {
                QVector<QVariant> chunk;
                int pos = mSentItemsCountPerGroup[gr];
                if (pos == mItemsByGroups[gr].size()) {
                    continue;
                }
                int sendingChunkSize = dataChunkSize;
                if (pos + dataChunkSize > mItemsByGroups[gr].size()) {
                    sendingChunkSize =  mItemsByGroups[gr].size() - pos;
                }
                listToSend.append(DataChunkType(mGroupsData[gr], mItemsByGroups[gr].mid(pos, sendingChunkSize)));
                mSentItemsCountPerGroup[gr] = pos + sendingChunkSize;
            }
        }
        emit dataChunkReady(listToSend);
        dataFile.close();
    } else {
        qCritical() << "Can not open file for reading: " << filePath;
    }
}

bool DataProvider::canFetch(int group)
{
    return mSentItemsCountPerGroup[group] < mItemsByGroups[group].size();
}

DataProvider::~DataProvider()
{
}

bool DataProvider::splitJsonToGroups(const QJsonDocument& json)
{
    if (mItemsByGroups.size() > 0) //done
        return true;

    QJsonArray jsonArray = json["roster"].toArray();
    if (jsonArray.size() == 0 ) {
        qCritical() <<"No elements in roster array";
        return false;
    }

    foreach(QJsonValue item, jsonArray) {
        int groupId = item.toObject().value("groupOrder").toInt();
        if (!mGroupsData.contains(groupId)) {
            QVariantMap groupData;
            groupData["groupOrder"] = groupId;
            groupData["group"] = item.toObject().value("group");
            groupData["id"] = item.toObject().value("id");
            mGroupsData[groupId] = groupData;
        }
        mItemsByGroups[groupId].push_back(item.toVariant());
    }
    return true;
}

void DataProvider::handleDownloadFinished(const QByteArray& data)
{
    QFile dataFile(filePath);
    if (dataFile.open(QFile::WriteOnly)) {
        dataFile.write(data);
        dataFile.close();
    } else {
        qCritical() << "Can not open file for writing: " << filePath;
    }
    QJsonDocument json = QJsonDocument::fromJson(data);
    //TODO: Check if json loaded without error
    //emit dataReady(json);
}
