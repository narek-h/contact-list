#include "downloader.h"

const QString urlString = "https://file.wowapp.me/owncloud/index.php/s/sGOXibS0ZSspQE8/download";

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    mNetworkManager =  new QNetworkAccessManager(this);
}

void Downloader::downloadDataFile()
{
    qDebug() << "starting download...";
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleRequestFinished(QNetworkReply*)));

    QNetworkRequest request(urlString);
    mNetworkManager->get(request);
}

void Downloader::handleRequestFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
         QJsonDocument d = QJsonDocument::fromJson(data);
         emit dataReady(d);
    }  else {
        qCritical() << "Error: " << QString::number(reply->error())
                    << "on file download";
        QByteArray data = reply->readAll();
        qDebug() << "received data ... \n" << data.data();
    }

    reply->deleteLater();
}
