#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QVariant>


typedef QPair<QVariant, QVector<QVariant>> DataChunkType;
typedef QVector<DataChunkType> dataChunkList;

class DataProvider : public QObject
{
    Q_OBJECT
    //Singletone
public:
    DataProvider(DataProvider const&) = delete;
    DataProvider& operator=(DataProvider const&) = delete;
    //Move ctor and assignment will not be generated.

    static DataProvider& getInstance();

    void fetchData(int group = -1);
    bool canFetch(int group);

signals:
    void dataChunkReady(const dataChunkList&);

private:
    DataProvider(QObject *parent = nullptr);
    ~DataProvider();

private slots:
    void handleDownloadFinished(const QByteArray&);
    bool splitJsonToGroups(const QJsonDocument& json);
private:
    static DataProvider mInstance;
    QMap<int, QVector<QVariant>> mItemsByGroups;
    QMap<int, QVariant> mGroupsData;
    QMap<int, int> mSentItemsCountPerGroup;

    QVariantMap mGrSizes;
};

#endif // DATAPROVIDER_H
