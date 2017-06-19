#ifndef NWSENDPOINTS_H
#define NWSENDPOINTS_H

#include <vector>
#include <memory>
#include <QByteArray>
#include <QStringBuilder>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "appoptions.h"

// Definitions for processing the different JSON files
#define NOJSONERROR 0
#define INVALIDJSONDOCUMENT -1
#define UNKNOWNJSONDOCUMENT -2
#define NOTPOINTSFORECAST -3
#define NOFORECASTVECTOR -4
#define NOPERIODSFOUND -5
#define NOTPOINTS -6
#define NOTPRODUCTS -7
#define NOTADFDOCUMENT -8
#define NOADFAVAILABLE -9
#define NOTHWODOCUMENT -10
#define NOHWOAVAILABLE -11

// Predefined QStrings for creating endpoints on the fly.
const QString BASE_ENDPOINT = "https://api.weather.gov";
const QString POINTS = "/points";
const QString FORECAST = "/forecast";
const QString HOURLY = "/hourly";
const QString STATIONS = "/stations";
const QString OBSERVATIONS = "/observations";
const QString CURRENT = "/current";
const QString ALERTS = "/alerts";
const QString PRODUCTS = "/products";
const QString LOCATIONS = "/locations";
const QString TYPES = "/types";

// Structure for urls and a few extra bits of url data
struct EndpointUrls {
    qreal latitude;
    qreal longitude;
    // The following will be extracted from the member
    // NWSEndpoints::extractUrlsFromPoints
    qint16 grid_x;          // if needed later.
    qint16 grid_y;          // if needed later.
    QString cwa;
    QString radar_station;  // 3 letter identifier
    QString city;           // closest city to forecast point
    QString state;          // state that forecast point is in
    QString offices;
    QString gridpoints;
    QString products_locations_types;
    QString zones_forecast;
    QString zones_forecast_forecast;
    QString zones_fire;
    QString zones_fire_forecast;
    QString zones_county;
    QString zones_county_forecast;
    // The following will be created in
    // NWSEndpoints::setPoint
    QString lat_lon;    // a GPS point in QString representation.
    QString points_base;
    QString points_forecast;
    QString points_forecast_hourly;
    QString points_stations;
    QString alerts_base;    // Caution! All alerts!
    QString alerts_point_active;
    // The following will need to be added after
    // the points forecast has been retrieved.
    QString updatedTime;
    QString products_types_AFD_locations;
    QString products_types_HWO_locations;
    QString latest_AFD;
    QString latest_HWO;
};

// Structure for storing urls of stations.
// It will, itself, be stored in a vector since there and many
// stations available for each forecast point.
struct StationEndpoints {
    QString stations;
    QString stations_observations;
    QString stations_observations_current;
};

// Structure for holding the forecast data.
// It will, itself, be stored in a vector.
struct PointsForecast {
    qint16 number;
    QString name;
    QString startTime;
    QString endTime;
    bool isDaytime;
    qint16 temperature;
    QString windSpeed;
    QString windDirection;
    QString icon;
    QString shortForecast;
    QString detailedForecast;
};

struct ActiveAlerts {
    QString event;
    QString headline;
    QString description;
};

class NWSEndpoints
{
public:
    NWSEndpoints();
    bool setPoint(qreal lat, qreal lon,
            EndpointUrls &endpoint_urls);
    qint16 createUrlsFromProducts(QByteArray &inbound_data,
            EndpointUrls &endpoint_urls);
    qint16 extractUrlsFromPoints(QByteArray &points_data,
            EndpointUrls &endpoint_urls);
    qint16 extractLatestAfdUrl(QByteArray &inbound_data,
            EndpointUrls &endpoint_urls);
    qint16 extractLatestHwoUrl( QByteArray &inbound_data,
            EndpointUrls &endpoint_urls);
    QString extractLatestAfd(QByteArray &inbound_data);
    QString extractLatestHwo(QByteArray &inbound_data);
    qint16 parsePointsForecast(QByteArray &inbound_data,
            EndpointUrls &endpoint_urls,
            std::vector<PointsForecast> &points_forecast);
    qint16 parseActiveAlerts(QByteArray &inbound_data,
            std::vector<ActiveAlerts> &active_alerts);

private:
    QString formatLatitudeString(qreal lat);
    QString formatLongitudeString(qreal lon);
};

#endif // NWSENDPOINTS_H
