#include "downloader.h"


Downloader::Downloader(QObject *parent) :QObject(parent)
{
    download_in_progress = false;
    setDefaultRequestInfo();
}

bool Downloader::busy()
{
    return download_in_progress;
}

bool Downloader::setDefaultRequestInfo()
{
    if(download_in_progress) return false;
    network_request.setRawHeader("Accept", "*/*, */*");
    network_request.setRawHeader("User-Agent", "QHWeather");
    network_request.setAttribute(
                QNetworkRequest::FollowRedirectsAttribute, true);
    return true;
}

bool Downloader::setAlternateRequestInfo(QNetworkRequest request)
{
    if(download_in_progress) return false;
    network_request = request;
    return true;
}

bool Downloader::requestDownload(DelayedRequest &delayed_request)
{
    if(download_in_progress) return false;
    network_url = delayed_request.url_string;
    if(!network_url.isValid()) return false;
    download_in_progress = true;
    extern QNetworkAccessManager *q_nam;
    return_data.caller_id = delayed_request.caller_id;
    return_data.caller_index = delayed_request.caller_index;
    return_data.caller_type = delayed_request.caller_type;
    return_data.data_requested = "";
    network_request.setUrl(network_url);
    network_reply = q_nam->get(network_request);
    connect(network_reply, network_reply->finished,
            this, readFullReply);
    return download_in_progress;
}

bool Downloader::requestDownload(QString url,
                                 qint16 index,
                                 qint16 type,
                                 qint16 id)
{
    if(download_in_progress) return false;
    network_url.setUrl(url);
    if(!network_url.isValid()) return false;
    extern QNetworkAccessManager *q_nam;
    download_in_progress = true;
    return_data.caller_id = id;
    return_data.caller_index = index;
    return_data.caller_type = type;
    return_data.data_requested = "";
    network_request.setUrl(network_url);
    network_reply = q_nam->get(network_request);
    network_reply->ignoreSslErrors();
    connect(network_reply, network_reply->finished,
            this, readFullReply);
    return download_in_progress;
}


bool Downloader::requestDownload(QUrl url,
                                 qint16 index,
                                 qint16 type,
                                 qint16 id)
{
    if(download_in_progress) return false;
    if(!url.isValid()) return false;
    extern QNetworkAccessManager *q_nam;
    download_in_progress = true;
    return_data.caller_id = id;
    return_data.caller_index = index;
    return_data.caller_type = type;
    return_data.data_requested = "";
    network_request.setUrl(url);
    network_reply = q_nam->get(network_request);
    network_reply->ignoreSslErrors();
    connect(network_reply, network_reply->finished,
            this, readFullReply);
    return download_in_progress;
}

bool Downloader::requestDownload(QNetworkRequest &request,
                                 qint16 index,
                                 qint16 type,
                                 qint16 id)
{
    if(download_in_progress) return false;
    extern QNetworkAccessManager *q_nam;
    download_in_progress = true;
    return_data.caller_id = id;
    return_data.caller_index = index;
    return_data.caller_type = type;
    return_data.data_requested = "";
    network_reply = q_nam->get(request);
    network_reply->ignoreSslErrors();
    connect(network_reply, network_reply->finished,
            this, readFullReply);
    return download_in_progress;
}

void Downloader::readFullReply()
{
    return_data.data_requested = network_reply->readAll();
    network_reply->deleteLater();
    download_in_progress = false;
    emit downloadRetrieved(return_data);
}
