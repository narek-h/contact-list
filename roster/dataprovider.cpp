#include "dataprovider.h"
#include "downloader.h"

#include <QStandardPaths>
#include <QJsonDocument>
#include <QFile>

const QString filePath = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[0] + "/data.json";

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
    QFile dataFile(filePath);
    if (!dataFile.exists()) {
        Downloader * down = new Downloader(this);
        down->downloadDataFile();
        connect(down, SIGNAL(downloadFinished(const QByteArray&)), this, SLOT(handleDownloadFinished(const QByteArray&)));
    } else if (dataFile.open(QFile::ReadOnly))  {
        QJsonDocument data = QJsonDocument::fromJson(dataFile.readAll());
        //TODO: Check if json loaded without error
        emit dataReady(data);
        dataFile.close();
    } else {
        qCritical() << "Can not open file for reading: " << filePath;
    }
}

DataProvider::~DataProvider()
{
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
    emit dataReady(json);
}
