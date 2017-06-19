#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QDialog>
#include <QFont>
#include <QFontDialog>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

const QString JSONFILENAME = "QHWeather.json";
const QString NORMANLOCATIONNAME = "Norman, OK";
const QString NORMANLATSTRING = "35.22";
const QString NORMANLONSTRING = "-97.44";
const qreal NORMANLATQREAL = 35.22;
const qreal NORMANLONQREAL = -97.44;

struct ForecastLocations {
    QString location_name;
    qreal latitude;
    qreal longitude;
    bool default_location;
};

struct Options {
    bool debug;
    // whether to load the forecast when the app starts.
    bool load_on_start;
    // This is the default location. It is coppied in later.
    QString location_name;
    qreal latitude;
    qreal longitude;
    // determines whether to download the icons or not.
    bool load_icons;
    // The following are for creating html on various pages
    QString name_align;
    QString name_size;
    QString name_color;
    QString name_line_height;
    QString detailed_align;
    QString detailed_size;
    QString detailed_color;
    // The following are for creating default fonts
    QString basic_font_family;
    qint16 basic_font_size;
    qint16 basic_font_weight;
    bool basic_font_italic;
    QString tab_font_family;
    qint16 tab_font_size;
    qint16 tab_font_weight;
    bool tab_font_italic;
    QString plain_text_font_family;
    qint16 plain_text_font_size;
    qint16 plain_text_font_weight;
    bool plain_text_font_italic;
    // The following are for setting for the CONUS radar
    qreal conus_h_scroll_ratio;
    qreal conus_v_scroll_ratio;
    qint32 conus_default_width;
    qint32 conus_default_height;
    qint32 conus_default_width_step;
    qint32 conus_default_height_step;
    // The following are the settings for the ridge radar
    qint32 ridge_default_width;
    qint32 ridge_default_height;
    // The following are for the alerts window
    QString event_align;
    QString event_size;
    QString event_color;
    QString event_line_height;
    QString alerts_plain_text_font_family;
    qint16 alerts_plain_text_font_size;
    qint16 alerts_plain_text_font_weight;
    bool alerts_plain_text_font_italic;

    Options() :
        debug(false),
        load_on_start(false),
        location_name(""),
        latitude(0),
        longitude(0),
        load_icons(true),
        name_align("left"),
        name_size("48"),
        name_color("#0000ff"),
        name_line_height("120"),
        detailed_align("left"),
        detailed_size("24"),
        detailed_color("#000000"),
        basic_font_family("MS Shell Dlg 2"),
        basic_font_size(16),
        basic_font_weight(50),
        basic_font_italic(false),
        tab_font_family("MS Shell Dlg 2"),
        tab_font_size(12),
        tab_font_weight(50),
        tab_font_italic(false),
        plain_text_font_family("Courier New"),
        plain_text_font_size(12),
        plain_text_font_weight(50),
        plain_text_font_italic(false),
        conus_h_scroll_ratio(0.4633),
        conus_v_scroll_ratio(0.5370),
        conus_default_width(3400),
        conus_default_height(1600),
        conus_default_width_step(845),
        conus_default_height_step(390),
        ridge_default_width(600),
        ridge_default_height(550),
        event_align("left"),
        event_size("24"),
        event_color("#0000ff"),
        event_line_height("32"),
        alerts_plain_text_font_family("Courier New"),
        alerts_plain_text_font_size(12),
        alerts_plain_text_font_weight(50),
        alerts_plain_text_font_italic(false)
    {}
};

namespace Ui {
class AppOptions;
}

class AppOptions : public QDialog
{
    Q_OBJECT

public:
    explicit AppOptions(QWidget *parent = 0);
    ~AppOptions();
    void loadProgramOptions();
    void saveProgramOptions();
    void initiateEditor();

private slots:
    void on_cancelPushButton_clicked();
    void on_okayPushButton_clicked();
    void on_basicFontPushButton_clicked();
    void on_tabFontPushButton_clicked();
    void on_plainTextFontPushButton_clicked();
    void on_alertsFontPushButton_clicked();
    void on_basicFontResetPushButton_clicked();
    void on_tabFontResetPushButton_clicked();
    void on_plainTextFontResetPushButton_clicked();
    void on_alertsFontResetPushButton_clicked();

private:
    Ui::AppOptions *ui;
    Options temp_options;

    void loadNormanOklahoma();
};

#endif // APPOPTIONS_H
