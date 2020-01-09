#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QJsonObject>


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

    void setFilter(const QString& text);

signals:
    void dataChunkReady(const dataChunkList&);

private:
    DataProvider(QObject *parent = nullptr);
    ~DataProvider();

private slots:
    void handleDownloadFinished(const QByteArray&);
    QMap<int, QVector<QJsonObject> > splitJsonToGroups(const QJsonDocument& json);
private:
    static DataProvider mInstance;
    QMap<int, QVariant> mGroupsData;
    QMap<int, int> mSentItemsCountPerGroup;

    QVariantMap mGrSizes;
    QString mFilter;
};

#endif // DATAPROVIDER_H
