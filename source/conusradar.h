#ifndef CONUSRADAR_H
#define CONUSRADAR_H

#include <QDialog>
#include <QLabel>
#include <QIcon>
#include <QMovie>
#include <QScrollBar>
#include <QScrollArea>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QSize>
#include <QSizePolicy>
#include <QResizeEvent>
#include "downloader.h"

#define CONUSZOOMMIN 0.25
#define CONUSZOOMMAX 2
#define CONUSZOOMSTEP 0.25
#define CONUSZOOMDEFAULT 1

const QString CONUSRADAR =
        "https://radar.weather.gov/Conus/Loop/NatLoop.gif";

namespace Ui {
class ConusRadar;
}

class ConusRadar : public QDialog
{
    Q_OBJECT

public:
    explicit ConusRadar(QWidget *parent = 0);
    ~ConusRadar();
    bool conusRadarEnabled(bool enable);

private slots:
    void on_playPausePushButton_clicked();
    void on_zoomInPushButton_clicked();
    void on_zoomOutPushButton_clicked();
    void on_refreshPushButton_clicked();

private:
    Ui::ConusRadar *ui;
    QLabel *conusLabel;
    QMovie *conusMovie;
    QBuffer *conusBuffer;
    Downloader *downloader;
    QScrollBar *conus_h_scroll_bar;
    QScrollBar *conus_v_scroll_bar;
    QIcon pause_icon;
    QIcon play_icon;
    QSize default_conus_size;
    qreal zoom_factor;
    bool conus_radar_enabled;
    bool use_initial_conus_values;

    void saveInitialScrollBarData();
    void setInitialScrollBarData();
    void refreshConusRadar();
    void processDownload(ReturnData data);
    void setupSceneSlider();
    void updateSceneSlider();
    void updateScene(qint16 number);
    void nextFrame();
    void zoomRadar();
};

#endif // CONUSRADAR_H
