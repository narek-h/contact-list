#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);

    void downloadDataFile();

signals:
    void dataReady(const QJsonDocument&);

public slots:
    void handleRequestFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* mNetworkManager;
};

#endif // DOWNLOADER_H
