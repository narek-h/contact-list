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
        qDebug() << "here";
        //TODO: make a  member to not recreate?
        Downloader * down = new Downloader(this);
        connect(down, SIGNAL(downloadFinished(const QByteArray&)), this, SLOT(handleDownloadFinished(const QByteArray&)));
        down->downloadDataFile();
        return;
    }

    QVector<int> groupsToFetch;
    if (group = -1) {//fetch all
        groupsToFetch.append(groupOrders);
    } else {
        groupsToFetch.append(group);
    }

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

        int i = 0;
        //Go to pos
        for(; (i < pos) && (i < groupTotalSize) ; ++i) {
            dataFile.readLine();
        }

        //Now populate the chunk
        int chunkEnd = std::min(pos + dataChunkSize, groupTotalSize);
        for (; i < chunkEnd; ++i) {
            QByteArray line = dataFile.readLine();
            QJsonDocument item = QJsonDocument::fromJson(line);
            if(item.isEmpty()) {
                qCritical() <<"Unexpected data from cache " <<line;
                continue;
            }
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
        listToSend.push_back(DataChunkType(mGroupsData[gr], chunk));
        mSentItemsCountPerGroup[gr] = pos + chunk.size();
        dataFile.close();
    }

    emit dataChunkReady(listToSend);
}

bool DataProvider::canFetch(int group)
{
    return mSentItemsCountPerGroup[group] < mGrSizes[QString::number(group)].toInt();
}

DataProvider::~DataProvider()
{
}

bool DataProvider::splitJsonToGroups(const QJsonDocument& json)
{
    if (mItemsByGroups.size() > 0) //done
        return true;

    QJsonArray jsonArray = json.object()["roster"].toArray();
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
        //Need to sort all the items here. Since we put only some portion in the View we need to sort the entire data
        foreach(int i , mItemsByGroups.keys()) {
            //To heavy. Try to optimize sorting.
            //            std::sort(mItemsByGroups[i].begin(), mItemsByGroups[i].end(), [](const QVariant& item1, const QVariant& item2)
            //            {
            //                return  true;//for now disabling
            //                if(item1.toMap()["account"].toMap()["firstName"].toString() >
            //                   item2.toMap()["account"].toMap()["firstName"].toString())
            //                    return false;
            //                return true;
            //            });
        }
    }
    return true;
}

void DataProvider::handleDownloadFinished(const QByteArray& data)
{
    qDebug() << "Download finished";
    QJsonDocument json = QJsonDocument::fromJson(data);
    bool done = splitJsonToGroups(json);


    QVariantMap grSizes;
    foreach(int i, mItemsByGroups.keys()) {
        grSizes[QString::number(i)] = mItemsByGroups[i].size();
        //Save items to files
        QString path = filePath + QString::number(i) + ".json";
        qDebug() << "writing to the fie " << path;
        QFile dataFile(path);
        if (dataFile.open(QFile::WriteOnly)) {
            foreach(QVariant item, mItemsByGroups[i]) {
                qDebug() <<"writing to file ...string" << QJsonDocument::fromVariant(item);
                dataFile.write(QJsonDocument::fromVariant(item).toJson(QJsonDocument::Compact));
                dataFile.write("\n"); //To be able to read by lines
            }
            dataFile.close();
        } else {
            qCritical() << "Can not open file for writing: " << path;
            done = false;
        }
    }
    if (done) {
        QSettings settings;
        settings.setValue("cacheTimeStamp", QDateTime::currentDateTime());
        settings.setValue("groupsSizesKey", grSizes);
        //Fetch/send first portion
        fetchData();
    }
}
