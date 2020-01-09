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

public:
    DataProvider(QObject *parent = nullptr);
    ~DataProvider();
    bool canFetch(int group);

    void setFilter(const QString& text);

signals:
    void dataChunkReady(const dataChunkList&);

private slots:
    void handleDownloadFinished(const QByteArray&);
    QMap<int, QVector<QJsonObject> > splitJsonToGroups(const QJsonDocument& json);
    void fetchData(int group = -1);
private:
    QMap<int, QVariant> mGroupsData;
    QMap<int, int> mSentItemsCountPerGroup;

    QVariantMap mGrSizes;
    QString mFilter;
};

#endif // DATAPROVIDER_H
