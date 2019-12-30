#include "dataprovider.h"
#include "downloader.h"

DataProvider::DataProvider(QObject *parent) : QObject(parent)
{
}

DataProvider& DataProvider::getInstance()
{
    static DataProvider mInstance;
    return mInstance;
}

void DataProvider::fetchData()
{
    Downloader * down = new Downloader(this);
    down->downloadDataFile();
    //For now just forwarding, will be cached etc..
    connect(down, SIGNAL(dataReady(const QJsonDocument&)), this, SIGNAL(dataReady(QJsonDocument)));
}
DataProvider::~DataProvider()
{
}

