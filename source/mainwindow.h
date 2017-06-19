#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <memory>
#include <queue>
#include <Qt>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QNetworkRequest>
#include <QString>
#include <QStringBuilder>
#include <QFont>
#include <QIcon>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextImageFormat>
#include <QTextFrameFormat>
#include <QUrl>
#include <QVariant>
#include <QMessageBox>
#include <QTime>
#include <QDateTime>
#include "downloader.h"
#include "nwsendpoints.h"
#include "appoptions.h"
#include "conusradar.h"
#include "alertswindow.h"
#include "ridgeradarlite.h"
#include "locationeditor.h"

#define NUM_DOWNLOADERS 8
/*
 * If the caller_type is an icon, then caller_id should indicate
 * its position in the forecast.
 */
#define TYPE_POINTS 1
#define TYPE_FORECAST 2
#define TYPE_STATIONS 3
#define TYPE_ALERTS 4
#define TYPE_PRODUCTS 5
#define TYPE_AFD 6
#define TYPE_HWO 7
#define TYPE_LATEST_ADF 8
#define TYPE_LATEST_HWO 9
#define TYPE_ICON 10

const QByteArray NWS_ACCEPT = "application/geo+json;version=1, image/jpeg, image/gif, image/png";
//const QByteArray DEFAULT_ACCEPT = "*/*, */*";
const QByteArray NWS_USER_AGENT = "QHWeather marisa@queerhill.com";
//const QByteArray DEFAULT_USER_AGENT = "QHWeather";

namespace Ui {
class MainWindow;
}

class MainWindow
        : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    AlertsWindow *alerts_window;
    AppOptions *app_options;
    LocationEditor *location_editor;
    QNetworkRequest network_request;
    QFont basic_font;
    QFont tab_font;
    QFont plain_text_font;
    QIcon clear_icon;
    QIcon alert_icon;
    std::vector<PointsForecast> points_forecast;
    std::vector<ActiveAlerts> active_alerts;
    std::unique_ptr<Downloader> *connection;
    std::vector<std::unique_ptr<Downloader>> vec;
    std::queue<DelayedRequest> request_queue;
    NWSEndpoints endpoints;
    EndpointUrls endpoint_urls;
    DelayedRequest delayed_request;
    qint16 number_active_alerts;
    bool forecast_updating;

    void loadUiFonts();
    void loadLocations();
    void processLocation(ProcessedLocation location);
    void processDownload(ReturnData data);
    qint16 updateRequests();
    void loadForecast();
    void pointsForecastToHtml(QString &detailed_forecast);
    void queueIcons();
    void userInputEnabled(bool updated_state);

private slots:
    void updateLocation(int index);
    void updateAndRefreshLocation();
    void on_reloadForecastPushButton_clicked();
    void on_radarPushButton_clicked();
    void on_warningPushButton_clicked();
    void on_ridgePushButton_clicked();
    void on_settingsPushButton_clicked();
    void on_addLocationPushButton_clicked();
    void on_editLocationPushButton_clicked();
    void on_deleteLocationPushButton_clicked();
};

#endif // MAINWINDOW_H
