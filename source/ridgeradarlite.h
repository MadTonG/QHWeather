#ifndef RIDGERADARLITE_H
#define RIDGERADARLITE_H

#include <iterator>
#include <QDialog>
#include <QBuffer>
#include <QString>
#include <QStringBuilder>
#include <QScrollBar>
#include <QNetworkReply>
#include <QLabel>
#include <QIcon>
#include <QMovie>
#include <QFont>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include "downloader.h"
#include "nwsendpoints.h"
#include "appoptions.h"

#define N0R "https://radar.weather.gov/ridge/lite/N0R/"
#define RIDGEZOOMMAX 3
#define RIDGEZOOMMIN 1
#define RIDGEZOOMSTEP 0.25
#define RIDGEZOOMDEFAULT 1

namespace Ui {
class RidgeRadarLite;
}

class RidgeRadarLite : public QDialog
{
    Q_OBJECT

public:
    explicit RidgeRadarLite(QWidget *parent = 0);
    ~RidgeRadarLite();
    void receiveRidgeLocations(EndpointUrls &urls);

private slots:
    void loadRadarComboBox(QString text);
    void ridgeRadarSelected(int index);
    void on_playPausePushButton_clicked();
    void on_zoomInPushButton_clicked();
    void on_zoomOutPushButton_clicked();
    void on_refreshPushButton_clicked();

private:
    Ui::RidgeRadarLite *ui;
    QLabel *ridgeLabel;
    QMovie *ridgeMovie;
    QBuffer *ridgeBuffer;
    Downloader *downloader;
    QScrollBar *ridge_h_scroll_bar;
    QScrollBar *ridge_v_scroll_bar;
    QFont basic_font;
    QString selected_ridge_radar_url;
    QString radar_station;
    QIcon pause_icon, play_icon;
    QSize ridge_default_size;
    QJsonDocument radar_location_document;
    QJsonObject state_json_object;
    EndpointUrls endpoint_urls;
    qreal zoom_factor;
    bool ridge_radar_lite_enabled;

    void refreshRidgeRadar();
    void processDownload(ReturnData data);
    void setupSceneSlider();
    void updateSceneSlider();
    void updateScene(qint32 number);
    void nextFrame();
    void zoomRadar();
    void loadRadarLocations();
    void loadStateComboBox();
};

#endif // RIDGERADARLITE_H
