#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>

class DataProvider : public QObject
{
    Q_OBJECT
    //Singletone
public:
    DataProvider(DataProvider const&) = delete;
    DataProvider& operator=(DataProvider const&) = delete;
    //Move ctor and assignment will not be generated.

    static DataProvider& getInstance();

    void fetchData();

signals:
    void dataReady(const QJsonDocument&);

private:
    DataProvider(QObject *parent = nullptr);
    ~DataProvider();


public slots:
private:
    static DataProvider mInstance;
};

#endif // DATAPROVIDER_H
