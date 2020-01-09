#include "dataprovider.h"
#include "downloader.h"

#include <QStandardPaths>
#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QtAlgorithms>
#include <QSettings>

const QString filePath = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0] + "/group_";
const QVector<int> groupOrders = {0, 1, 2}; //Hardcoding the list for simplicity but the code can be adjusted to extract this from the data.
const int dataChunkSize = 100;
const QString cacheTimeStamp = "cacheTimeStamp";
const QString groupsSizesKey = "groupsSizesKey";
const int cacheExpirationInSecs = 3600;

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
    QSettings settings;
    if (!((settings.contains(cacheTimeStamp)) &&
          (QDateTime(settings.value(cacheTimeStamp).toDateTime()).secsTo(QDateTime::currentDateTime()) < cacheExpirationInSecs))) { //No cache or expired
        //TODO: make a  member to not recreate?
        Downloader * down = new Downloader(this);
        connect(down, SIGNAL(downloadFinished(const QByteArray&)), this, SLOT(handleDownloadFinished(const QByteArray&)));
        down->downloadDataFile();
        return;
    }

    QVector<int> groupsToFetch;
    if (group == -1) {//fetch all
        groupsToFetch.append(groupOrders);
    } else {
        groupsToFetch.append(group);
    }

    bool canAddMore = false;
    dataChunkList listToSend;
    mGrSizes = settings.value(groupsSizesKey).toMap();
    foreach(int gr, groupsToFetch) {

        QString path = filePath + QString::number(gr) + ".json"; //make a function
        QFile dataFile(path);
        if (!dataFile.open(QFile::ReadOnly))  {
            qCritical() <<"Can't open file for reading " << path;
        }

        QVector<QVariant> chunk;
        QString grStr = QString::number(gr);
        int pos = mSentItemsCountPerGroup.contains(gr) ? mSentItemsCountPerGroup[gr] : 0;
        int groupTotalSize = mGrSizes[grStr].toInt();

        if (pos >= groupTotalSize ) {
            continue;
        }

        //Go to pos
        for(int i = 0; (i < pos) && (i < groupTotalSize) ; ++i) {
            dataFile.readLine();
        }

        //Now populate the chunk
        int chunkEnd = std::min(pos + dataChunkSize, groupTotalSize);
        for (; pos < chunkEnd; ++pos) {
            QByteArray line = dataFile.readLine();
            QJsonDocument item = QJsonDocument::fromJson(line);
            if(item.isEmpty()) {
                qCritical() <<"Unexpected data from cache " <<line;
                continue;
            }
            bool toAdd = false;
            if (mFilter.size() > 1) {
                if (item.object()["account"].toObject()["firstName"].toString().contains(mFilter)) {
                    toAdd = true;
                }
            } else {
                toAdd = true;
            }
            if (toAdd) {
                chunk.push_back(item.toVariant());
                //Create group
                if (!mGroupsData.contains(gr)) {
                    QVariantMap groupData;
                    groupData["groupOrder"] = item.object().value("groupOrder").toInt();
                    groupData["group"] = item.object().value("group");
                    groupData["id"] = item.object().value("id");
                    mGroupsData[gr] = groupData;
                }
            }
        }

        listToSend.push_back(DataChunkType(mGroupsData[gr], chunk));
        //TODO: change to pos not sent items
        mSentItemsCountPerGroup[gr] = pos;// + chunk.size();
        dataFile.close();
        if ((chunk.size() < dataChunkSize) && (pos < groupTotalSize)) {
            canAddMore = true;
        }
    }

    emit dataChunkReady(listToSend);
    if (canAddMore) {
        fetchData();
    }
}

bool DataProvider::canFetch(int group)
{
    return mSentItemsCountPerGroup[group] < mGrSizes[QString::number(group)].toInt();
}

DataProvider::~DataProvider()
{
}

QMap<int, QVector<QJsonObject>> DataProvider::splitJsonToGroups(const QJsonDocument& json)
{
    QMap<int, QVector<QJsonObject>> itemsByGroups;
    QJsonArray jsonArray = json.object()["roster"].toArray();
    for (auto item : jsonArray) {
        int groupId = item.toObject().value("groupOrder").toInt();
        if (!mGroupsData.contains(groupId)) {
            QVariantMap groupData;
            groupData["groupOrder"] = groupId;
            groupData["group"] = item.toObject().value("group");
            groupData["id"] = item.toObject().value("id");
            mGroupsData[groupId] = groupData;
        }
        itemsByGroups[groupId].push_back(item.toObject());
    }

    foreach(int i , itemsByGroups.keys()) {
        auto& groupItems = itemsByGroups[i];

        //Need to sort all the items here. Since we put only some portion in the View we need to sort the entire data
        std::sort(groupItems.begin(), groupItems.end(), [](const QJsonObject& item1, const QJsonObject& item2)
        {
            QString firstName1 = item1["account"].toObject()["firstName"].toString();
            QString firstName2 = item2["account"].toObject()["firstName"].toString();
            if( firstName1 < firstName2) {
                return true;
            } else if (firstName1 == firstName2) { //Checking separately is more optimal, not always extracting lastName
                return item1["account"].toObject()["lastName"].toString() < item2["account"].toObject()["lastName"].toString();
            }
            return false;
        });
    }

    if (itemsByGroups.empty()) {
        qCritical() << "No elements in roster array";
    }

    return itemsByGroups;
}

void DataProvider::handleDownloadFinished(const QByteArray& data)
{
    QJsonDocument json = QJsonDocument::fromJson(data);
    auto itemsByGroups = splitJsonToGroups(json);

    QVariantMap grSizes;

    bool success = true;
    foreach(int i, itemsByGroups.keys()) {
        grSizes[QString::number(i)] = itemsByGroups[i].size();
        //Save items to files
        QString path = filePath + QString::number(i) + ".json";        
        QFile dataFile(path);
        if (dataFile.open(QFile::WriteOnly)) {
            foreach(auto& item, itemsByGroups[i]) {
                //qDebug() <<"writing to file ...string" << QJsonDocument(item);
                dataFile.write(QJsonDocument(item).toJson(QJsonDocument::Compact));
                dataFile.write("\n"); //To be able to read by lines
            }
            dataFile.close();
        } else {
            qCritical() << "Can not open file for writing: " << path;
            success = false;
        }
    }
    if (success) {
        QSettings settings;
        settings.setValue("cacheTimeStamp", QDateTime::currentDateTime());
        settings.setValue("groupsSizesKey", grSizes);
        //Fetch/send first portion
        fetchData();
    }
}

void DataProvider::setFilter(const QString& text)
{
    mFilter = text;
    mSentItemsCountPerGroup.clear();
}
