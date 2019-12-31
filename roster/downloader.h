#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QByteArray>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);

    void downloadDataFile();

signals:
    void downloadFinished(const QByteArray&);

public slots:
    void handleRequestFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* mNetworkManager;
};

#endif // DOWNLOADER_H
