#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <Qt>
#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "appoptions.h"

struct DelayedRequest {
    qint16 caller_index;
    qint16 caller_type;
    qint16 caller_id;
    QString url_string;
};

struct ReturnData {
    qint16 caller_index;
    qint16 caller_type;
    qint16 caller_id;
    QByteArray data_requested;
};

class Downloader : public QObject
{
    Q_OBJECT
private:
    bool download_in_progress;
    ReturnData return_data;
    QNetworkRequest network_request;
    QNetworkReply *network_reply;
    QUrl network_url;

    void readFullReply();

public:
    explicit Downloader(QObject *parent = 0);
    bool busy();
    bool setDefaultRequestInfo();
    bool setAlternateRequestInfo(QNetworkRequest request);
    bool requestDownload(DelayedRequest &delayed_request);
    bool requestDownload(QString url,
                         qint16 index = -1,
                         qint16 type = -1,
                         qint16 id = -1);
    bool requestDownload(QUrl url,
                         qint16 index = -1,
                         qint16 type = -1,
                         qint16 id = -1);
    bool requestDownload(QNetworkRequest &request,
                         qint16 index = -1,
                         qint16 type = -1,
                         qint16 id = -1);

signals:
    void downloadRetrieved(ReturnData data);

public slots:

};

#endif // DOWNLOADER_H
