#include "nwsendpoints.h"
#include <QDebug>

NWSEndpoints::NWSEndpoints()
{
}

bool NWSEndpoints::setPoint(qreal lat,
                            qreal lon,
                            EndpointUrls &endpoint_urls)
{
    bool was_successful = false;
    if(lat < 15 || lat > 75) return was_successful;
    if(lon < -180 || lon > -60) return was_successful;
    endpoint_urls.latitude = lat;
    endpoint_urls.longitude = lon;
    QString lat_string = formatLatitudeString(lat);
    QString lon_string = formatLongitudeString(lon);
    QString lat_lon = "/" % lat_string % "," % lon_string;
    endpoint_urls.lat_lon = lat_lon;
    endpoint_urls.points_base = BASE_ENDPOINT %
            POINTS % lat_lon;
    endpoint_urls.points_forecast = BASE_ENDPOINT %
            POINTS % lat_lon % FORECAST;
    endpoint_urls.points_forecast_hourly = BASE_ENDPOINT %
            POINTS % lat_lon % FORECAST % HOURLY;
    endpoint_urls.points_stations = BASE_ENDPOINT %
            POINTS % lat_lon % STATIONS;
    was_successful = true;
    endpoint_urls.alerts_base = BASE_ENDPOINT % ALERTS;
    endpoint_urls.alerts_point_active =
            endpoint_urls.alerts_base %
            "?point=" % lat_string % "," % lon_string %
            "&active=1";
    return was_successful;
}

qint16 NWSEndpoints::createUrlsFromProducts(
        QByteArray &inbound_data,
        EndpointUrls &endpoint_urls)
{
    QJsonDocument products_document =
            QJsonDocument::fromJson(inbound_data);
    if(products_document.isNull()) return INVALIDJSONDOCUMENT;
    if(products_document.isEmpty()) return INVALIDJSONDOCUMENT;
    if(!products_document.isObject()) return UNKNOWNJSONDOCUMENT;
    QJsonObject document_object = products_document.object();
    if(document_object.isEmpty()) return NOTPRODUCTS;
    QJsonArray features_array =
            document_object.value("features").toArray();
    if(features_array.isEmpty()) return NOTPRODUCTS;
    int length = features_array.size();
    QString product;
    QString afd = "AFD";
    QString hwo = "HWO";
    QJsonObject product_object;
    for(int x=0; x<length; ++x) {
        product_object = features_array[x].toObject();
        product = product_object.value("productCode").toString();
        if(product == afd)
            endpoint_urls.products_types_AFD_locations =
                    BASE_ENDPOINT % PRODUCTS % TYPES
                    % "/" % afd % LOCATIONS
                    % "/" % endpoint_urls.cwa;
        if(product == hwo)
            endpoint_urls.products_types_HWO_locations =
                    BASE_ENDPOINT % PRODUCTS % TYPES
                    % "/" % hwo % LOCATIONS
                    % "/" % endpoint_urls.cwa;
    }
    return NOJSONERROR;
}

qint16 NWSEndpoints::extractUrlsFromPoints(
        QByteArray &points_data,
        EndpointUrls &endpoint_urls)
{
    QJsonDocument points_document =
            QJsonDocument::fromJson(points_data);
    if(points_document.isNull()) return INVALIDJSONDOCUMENT;
    if(points_document.isEmpty()) return INVALIDJSONDOCUMENT;
    if(!points_document.isObject()) return UNKNOWNJSONDOCUMENT;
    QJsonObject document_object = points_document.object();
    if(document_object.isEmpty()) return NOTPOINTS;
    QJsonObject properties_object =
            document_object.value("properties").toObject();
    if(properties_object.isEmpty()) return NOTPOINTS;
    endpoint_urls.grid_x =
            properties_object.value("gridX").toInt();
    endpoint_urls.grid_y =
            properties_object.value("gridY").toInt();
    endpoint_urls.cwa =
            properties_object.value("cwa").toString();
    endpoint_urls.products_locations_types =
            BASE_ENDPOINT % PRODUCTS % LOCATIONS
            % "/" % endpoint_urls.cwa % TYPES;
    QString radar_call_letters;
    radar_call_letters =
            properties_object.value("radarStation").toString();
    // only get the three letter identifier for the radar
    if(radar_call_letters >= 3)
        endpoint_urls.radar_station =
                radar_call_letters.right(3);
    else endpoint_urls.radar_station = radar_call_letters;
    endpoint_urls.offices =
            properties_object.value("forecastOffice").toString();
    endpoint_urls.gridpoints =
            properties_object.value("forecastGridData")
            .toString();
    endpoint_urls.zones_forecast =
            properties_object.value("forecastZone").toString();
    endpoint_urls.zones_forecast_forecast =
            endpoint_urls.zones_forecast % FORECAST;
    endpoint_urls.zones_fire =
            properties_object.value("fireWeatherZone")
            .toString();
    endpoint_urls.zones_fire_forecast =
            endpoint_urls.zones_fire % FORECAST;
    endpoint_urls.zones_county =
            properties_object.value("county").toString();
    endpoint_urls.zones_county_forecast =
            endpoint_urls.zones_county % FORECAST;
    // from relative location, the city and state data.
    QJsonObject relative_location_object;
    relative_location_object =
            properties_object.value("relativeLocation")
            .toObject();
    QJsonObject properties2_object;
    properties2_object =
            relative_location_object.value("properties")
            .toObject();
    endpoint_urls.city =
            properties2_object.value("city").toString();
    endpoint_urls.state =
            properties2_object.value("state").toString();
    return NOJSONERROR;
}

qint16 NWSEndpoints::extractLatestAfdUrl(
        QByteArray &inbound_data,
        EndpointUrls &endpoint_urls)
{
    QJsonDocument adf_document =
            QJsonDocument::fromJson(inbound_data);
    if(adf_document.isNull()) return INVALIDJSONDOCUMENT;
    if(!adf_document.isObject()) return UNKNOWNJSONDOCUMENT;
    QJsonObject document_object = adf_document.object();
    if(document_object.isEmpty()) return NOTADFDOCUMENT;
    QJsonArray features_array =
            document_object.value("features").toArray();
    if(features_array.isEmpty()) return NOADFAVAILABLE;
    QJsonObject latest_adf = features_array[0].toObject();
    endpoint_urls.latest_AFD =
            latest_adf.value("@id").toString();
    return NOJSONERROR;
}

qint16 NWSEndpoints::extractLatestHwoUrl(
        QByteArray &inbound_data,
        EndpointUrls &endpoint_urls)
{
    QJsonDocument hwo_document =
            QJsonDocument::fromJson(inbound_data);
    if(hwo_document.isNull()) return INVALIDJSONDOCUMENT;
    if(!hwo_document.isObject()) return UNKNOWNJSONDOCUMENT;
    QJsonObject document_object = hwo_document.object();
    if(document_object.isEmpty()) return NOTHWODOCUMENT;
    QJsonArray features_array =
            document_object.value("features").toArray();
    if(features_array.isEmpty()) return NOHWOAVAILABLE;
    QJsonObject latest_hwo = features_array[0].toObject();
    endpoint_urls.latest_HWO =
            latest_hwo.value("@id").toString();
    return NOJSONERROR;
}

QString NWSEndpoints::extractLatestAfd(QByteArray &inbound_data)
{
    QString to_return = "";
    QJsonDocument adf_document =
            QJsonDocument::fromJson(inbound_data);
    QJsonObject document_object = adf_document.object();
    to_return = document_object.value("productText").toString();
    return to_return;
}

QString NWSEndpoints::extractLatestHwo(QByteArray &inbound_data)
{
    QString to_return = "";
    QJsonDocument hwo_document =
            QJsonDocument::fromJson(inbound_data);
    QJsonObject document_object = hwo_document.object();
    to_return = document_object.value("productText").toString();
    return to_return;
}

qint16 NWSEndpoints::parsePointsForecast(
        QByteArray &inbound_data,
        EndpointUrls &endpoint_urls,
        std::vector<PointsForecast> &points_forecast)
{
    QJsonDocument forecast_document =
            QJsonDocument::fromJson(inbound_data);
    if(forecast_document.isNull()) return INVALIDJSONDOCUMENT;
    if(!forecast_document.isObject()) return UNKNOWNJSONDOCUMENT;
    QJsonObject document_object = forecast_document.object();
    if(document_object.isEmpty()) return NOTPOINTSFORECAST;
    QJsonObject properties_object =
            document_object.value("properties").toObject();
    if(properties_object.isEmpty()) return NOTPOINTSFORECAST;
    endpoint_urls.updatedTime =
            properties_object.value("updated").toString();
    QJsonArray periods_array =
            properties_object.value("periods").toArray();
    if(periods_array.isEmpty()) return NOTPOINTSFORECAST;
    qint16 periods = periods_array.size();
    if(periods <= 0) return NOPERIODSFOUND;
    QJsonObject period;
    PointsForecast forecast;
    for(int x=0; x<periods; ++x) {
        period = periods_array.at(x).toObject();
        forecast = {};
        forecast.number = period.value("number").toInt();
        forecast.name = period.value("name").toString();
        forecast.startTime =
                period.value("startTime").toString();
        forecast.endTime = period.value("endTime").toString();
        forecast.isDaytime = period.value("isDaytime").toBool();
        forecast.temperature =
                period.value("temperature").toInt();
        forecast.windSpeed =
                period.value("windSpeed").toString();
        forecast.windDirection =
                period.value("windDirection").toString();
        forecast.icon = period.value("icon").toString();
        forecast.shortForecast =
                period.value("shortForecast").toString();
        forecast.detailedForecast =
                period.value("detailedForecast").toString();
        points_forecast.push_back(forecast);
    }
    return NOJSONERROR;
}

qint16 NWSEndpoints::parseActiveAlerts(QByteArray &inbound_data,
        std::vector<ActiveAlerts> &active_alerts)
{
    qint16 return_value;
    ActiveAlerts alerts;
    QJsonDocument alerts_document =
            QJsonDocument::fromJson(inbound_data);
    QJsonObject document_object = alerts_document.object();
    QJsonArray features_array =
            document_object.value("features").toArray();
    return_value = features_array.size();
    if(return_value <= 0) return return_value;
    QJsonObject properties_object;
    QJsonObject array_object;
    for(qint16 x=0; x<return_value; ++x) {
        array_object = features_array.at(x).toObject();
        properties_object =
                array_object.value("properties").toObject();
        alerts = {};
        alerts.event =
                properties_object.value("event").toString();
        alerts.headline =
                properties_object.value("headline").toString();
        alerts.description =
                properties_object.value("description")
                .toString();
        active_alerts.push_back(alerts);
    }
    return return_value;
}

QString NWSEndpoints::formatLatitudeString(qreal lat)
{
    int proper_length = 7;
    QString latitude_string = QString::number(lat, 'f', 6);
    latitude_string.truncate(proper_length);
    QString::reverse_iterator iter;
    for(iter = latitude_string.rbegin();
        iter != latitude_string.rend();
        iter++) {
        if(*iter == '0') *iter = ' ';
        else break;
    }
    latitude_string = latitude_string.trimmed();
    return latitude_string;
}

QString NWSEndpoints::formatLongitudeString(qreal lon)
{
    int proper_length;
    if(lon < 100.0) proper_length = 8;
    else proper_length = 9;
    QString longitude_string = QString::number(lon, 'f', 6);
    longitude_string.truncate(proper_length);
    QString::reverse_iterator iter;
    for(iter = longitude_string.rbegin();
        iter != longitude_string.rend();
        iter++) {
        if(*iter == '0') *iter = ' ';
        else break;
    }
    longitude_string = longitude_string.trimmed();
    return longitude_string;
}
