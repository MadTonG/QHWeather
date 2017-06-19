#include "appoptions.h"
#include "ui_appoptions.h"

qreal conus_h_scroll_ratio;
qreal conus_v_scroll_ratio;
qint32 conus_default_width;
qint32 conus_default_height;
qint32 conus_default_width_step;
qint32 conus_default_height_step;

/* Options options; and
 * std::vector<forecastLocations> forecast_locations;
 * are used by various classes around the program. As such,
 * they are global variables that can be accessed by other
 * classes as follows:
 * extern Options options; and
 * extern std::vector<ForecastLocations> forecast_locations;
 */
Options options;
std::vector<ForecastLocations> forecast_locations;
QNetworkAccessManager *q_nam;

AppOptions::AppOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppOptions)
{
    ui->setupUi(this);
    q_nam = new QNetworkAccessManager(this);
}

AppOptions::~AppOptions()
{
    delete ui;
}

void AppOptions::loadProgramOptions()
{
    QFile file;
    QByteArray data;
    file.setFileName(JSONFILENAME);
    bool success = file.open(QFile::ReadOnly);
    if(!success) {
        // The Options struct will have its own defaults,
        // but there will be no location defaults.
        // We have to create one.
        loadNormanOklahoma();
        return;
    }
    data = file.readAll();
    if(data.length() <= 0) {
        // Something went wrong, close file and use defaults.
        loadNormanOklahoma();
        file.close();
        return;
    }
    // From this point on, we'll assume our json file is correct.
    QJsonDocument json_document = QJsonDocument::fromJson(data);
    QJsonObject document_object = json_document.object();
    QJsonObject opt =
            document_object.value("options").toObject();
    QJsonArray loc =
            document_object.value("locations").toArray();
    options.load_on_start = opt.value("loadOnStart").toBool();
    options.load_icons = opt.value("loadIcons").toBool();
    options.name_align = opt.value("nameAlign").toString();
    options.name_size = opt.value("nameSize").toString();
    options.name_color = opt.value("nameColor").toString();
    options.name_line_height =
            opt.value("nameLineHeight").toString();
    options.detailed_align =
            opt.value("detailedAlign").toString();
    options.detailed_size =
            opt.value("detailedSize").toString();
    options.detailed_color =
            opt.value("detailedColor").toString();
    options.basic_font_family =
            opt.value("basicFontFamily").toString();
    options.basic_font_size =
            opt.value("basicFontSize").toInt();
    options.basic_font_weight =
            opt.value("basicFontWeight").toInt();
    options.basic_font_italic =
            opt.value("basicFontItalic").toBool();
    options.tab_font_family =
            opt.value("tabFontFamily").toString();
    options.tab_font_size =
            opt.value("tabFontSize").toInt();
    options.tab_font_weight =
            opt.value("tabFontWeight").toInt();
    options.tab_font_italic =
            opt.value("tabFontItalic").toBool();
    options.plain_text_font_family =
            opt.value("plainTextFontFamily").toString();
    options.plain_text_font_size =
            opt.value("plainTextFontSize").toInt();
    options.plain_text_font_weight =
            opt.value("plainTextFontWeight").toInt();
    options.plain_text_font_italic =
            opt.value("plainTextFontItalic").toBool();
    options.conus_h_scroll_ratio =
            opt.value("conusHScrollRatio").toDouble();
    options.conus_v_scroll_ratio =
            opt.value("conusVScrollRatio").toDouble();
    options.conus_default_width =
            opt.value("conusDefaultWidth").toInt();
    options.conus_default_height =
            opt.value("conusDefaultHeight").toInt();
    options.conus_default_width_step =
            opt.value("conusDefaultWidthStep").toInt();
    options.conus_default_height_step =
            opt.value("conusDefaultHeightStep").toInt();
    options.event_align =
            opt.value("eventAlign").toString();
    options.event_size =
            opt.value("eventSize").toString();
    options.event_color =
            opt.value("eventColor").toString();
    options.event_line_height =
            opt.value("eventLineHeight").toString();
    options.alerts_plain_text_font_family =
            opt.value("alertsPlainTextFontFamily").toString();
    options.alerts_plain_text_font_size =
            opt.value("alertsPlainTextFontSize").toInt();
    options.alerts_plain_text_font_weight =
            opt.value("alertsPlainTextFontWeight").toInt();
    options.alerts_plain_text_font_italic =
            opt.value("alertsPlainTextFontItalic").toBool();
    QJsonObject this_loc;
    ForecastLocations temp;
    for(int x=0; x<loc.size(); ++x) {
        this_loc = loc.at(x).toObject();
        temp = {};
        temp.location_name =
                this_loc.value("locationName").toString();
        temp.latitude = this_loc.value("latitude").toDouble();
        temp.longitude = this_loc.value("longitude").toDouble();
        temp.default_location =
                this_loc.value("defaultLocation").toBool();
        forecast_locations.push_back(temp);
    }
}

void AppOptions::saveProgramOptions()
{
    QJsonObject opt;
    // Store data from the Options struct.
    opt.insert("loadOnStart", QJsonValue(options.load_on_start));
    opt.insert("loadIcons", QJsonValue(options.load_icons));
    opt.insert("nameAlign", QJsonValue(options.name_align));
    opt.insert("nameSize", QJsonValue(options.name_size));
    opt.insert("nameColor", QJsonValue(options.name_color));
    opt.insert("nameLineHeight",
               QJsonValue(options.name_line_height));
    opt.insert("detailedAlign",
               QJsonValue(options.detailed_align));
    opt.insert("detailedSize",
               QJsonValue(options.detailed_size));
    opt.insert("detailedColor",
               QJsonValue(options.detailed_color));
    opt.insert("basicFontFamily",
               QJsonValue(options.basic_font_family));
    opt.insert("basicFontSize",
               QJsonValue(options.basic_font_size));
    opt.insert("basicFontWeight",
               QJsonValue(options.basic_font_weight));
    opt.insert("basicFontItalic",
               QJsonValue(options.basic_font_italic));
    opt.insert("tabFontFamily",
               QJsonValue(options.tab_font_family));
    opt.insert("tabFontSize", QJsonValue(options.tab_font_size));
    opt.insert("tabFontWeigt",
               QJsonValue(options.tab_font_weight));
    opt.insert("tabFontItalic",
               QJsonValue(options.tab_font_italic));
    opt.insert("plainTextFontFamily",
               QJsonValue(options.plain_text_font_family));
    opt.insert("plainTextFontSize",
               QJsonValue(options.plain_text_font_size));
    opt.insert("plainTextFontWeight",
               QJsonValue(options.plain_text_font_weight));
    opt.insert("plainTextFontItalic",
               options.plain_text_font_italic);
    opt.insert("conusHScrollRatio",
               QJsonValue(options.conus_h_scroll_ratio));
    opt.insert("conusVScrollRatio",
               QJsonValue(options.conus_v_scroll_ratio));
    opt.insert("conusDefaultWidth",
               QJsonValue(options.conus_default_width));
    opt.insert("conusDefaultHeight",
               QJsonValue(options.conus_default_height));
    opt.insert("conusDefaultWidthStep",
               QJsonValue(options.conus_default_width_step));
    opt.insert("conusDefaultHeightStep",
               QJsonValue(options.conus_default_height_step));
    opt.insert("eventAlign", QJsonValue(options.event_align));
    opt.insert("eventSize", QJsonValue(options.event_size));
    opt.insert("eventColor", QJsonValue(options.event_color));
    opt.insert("eventLineHeight",
               QJsonValue(options.event_line_height));
    opt.insert("alertsPlainTextFontFamily",
               QJsonValue(
                   options.alerts_plain_text_font_family));
    opt.insert("alertsPlainTextFontSize",
               QJsonValue(options.alerts_plain_text_font_size));
    opt.insert("alertsPlainTextFontWeight",
               QJsonValue(
                   options.alerts_plain_text_font_weight));
    opt.insert("alertsPlainTextFontItalic",
               QJsonValue(
                   options.alerts_plain_text_font_italic));
    // Store the forecast locations.
    QJsonArray json_locations;
    for(int x=0; x<forecast_locations.size(); ++x) {
        QJsonObject loc = QJsonObject();
        loc.insert("locationName",
                   QJsonValue(
                       forecast_locations[x].location_name));
        loc.insert("latitude",
                   QJsonValue(forecast_locations[x].latitude));
        loc.insert("longitude",
                   QJsonValue(forecast_locations[x].longitude));
        loc.insert("defaultLocation",
                   QJsonValue(
                       forecast_locations[x].default_location));
        json_locations.append(QJsonValue(loc));
    }
    QJsonObject document_object = QJsonObject();
    document_object.insert("options", QJsonValue(opt));
    document_object.insert("locations",
                           QJsonValue(json_locations));
    QJsonDocument json_document = QJsonDocument(document_object);
    QFile file;
    file.setFileName(JSONFILENAME);
    file.open(QFile::WriteOnly);
    file.write(json_document.toJson());
    file.close();
}

void AppOptions::loadNormanOklahoma()
{
    /*
     * If this is the first run of the program, it won't have
     * any location information entered into it. We are going
     * to use Norman Oklahoma as the default town so that
     * the user can see what the program can do prior to having
     * to go through the trouble of setting up locations for
     * themselves.
     */
    ForecastLocations location;
    location.location_name = NORMANLOCATIONNAME;
    location.latitude = NORMANLATQREAL;
    location.longitude = NORMANLONQREAL;
    location.default_location = true;
    /*
     * Copy the selected default location to options.
     * This is necessary so that the program refresh will work.
     * This is usually done elsewhere in the program, but we
     * must do it since we aren't calling the other method where
     * this would normally be done.
     */
    options.location_name = location.location_name;
    options.latitude = location.latitude;
    options.longitude = location.longitude;
    /*
     * Finally, we want to save the location in the
     * forecast_locations vector so that it will be properly
     * displayed as an available location that can be selected
     * by the user
     */
    forecast_locations.push_back(location);
}

void AppOptions::initiateEditor()
{
    // first: make a copy of the options
    temp_options = options;

    // second: set the ui to the temp options.
    ui->loadOnStartCheckBox->setChecked(
                temp_options.load_on_start);
    ui->loadIconsCheckBox->setChecked(
                temp_options.load_icons);
    ui->debugCheckBox->setChecked(
                temp_options.debug);
}

void AppOptions::on_cancelPushButton_clicked()
{
    this->reject();
}

void AppOptions::on_okayPushButton_clicked()
{
    temp_options.load_icons =
            ui->loadIconsCheckBox->isChecked();
    temp_options.load_on_start =
            ui->loadOnStartCheckBox->isChecked();
    temp_options.debug = ui->debugCheckBox->isChecked();
    options = temp_options;
    this->accept();
}

void AppOptions::on_basicFontPushButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,
                        QFont(temp_options.basic_font_family,
                              temp_options.basic_font_size,
                              temp_options.basic_font_weight,
                              temp_options.basic_font_italic),
                        this);
    if(ok) {
        temp_options.basic_font_family = font.family();
        temp_options.basic_font_size = font.pointSize();
        temp_options.basic_font_weight = font.weight();
        temp_options.basic_font_italic = font.italic();
    }
}

void AppOptions::on_tabFontPushButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,
                        QFont(temp_options.tab_font_family,
                              temp_options.tab_font_size,
                              temp_options.tab_font_weight,
                              temp_options.tab_font_italic),
                        this);
    if(ok) {
        temp_options.tab_font_family = font.family();
        temp_options.tab_font_size = font.pointSize();
        temp_options.tab_font_weight = font.weight();
        temp_options.tab_font_italic = font.italic();
    }
}

void AppOptions::on_plainTextFontPushButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,
                    QFont(temp_options.plain_text_font_family,
                          temp_options.plain_text_font_size,
                          temp_options.plain_text_font_weight,
                          temp_options.plain_text_font_italic),
                        this);
    if(ok) {
        temp_options.plain_text_font_family = font.family();
        temp_options.plain_text_font_size = font.pointSize();
        temp_options.plain_text_font_weight = font.weight();
        temp_options.plain_text_font_italic = font.italic();
    }
}

void AppOptions::on_alertsFontPushButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,
                                      QFont(
                    temp_options.alerts_plain_text_font_family,
                    temp_options.alerts_plain_text_font_size,
                    temp_options.alerts_plain_text_font_weight,
                    temp_options.alerts_plain_text_font_italic),
                                      this);
    if(ok) {
        temp_options.alerts_plain_text_font_family =
                font.family();
        temp_options.alerts_plain_text_font_size =
                font.pointSize();
        temp_options.alerts_plain_text_font_weight =
                font.weight();
        temp_options.alerts_plain_text_font_italic =
                font.italic();
    }
}

void AppOptions::on_basicFontResetPushButton_clicked()
{
    Options reset_options;
    temp_options.basic_font_family =
            reset_options.basic_font_family;
    temp_options.basic_font_size =
            reset_options.basic_font_size;
    temp_options.basic_font_weight =
            reset_options.basic_font_weight;
    temp_options.basic_font_italic =
            reset_options.basic_font_italic;
}

void AppOptions::on_tabFontResetPushButton_clicked()
{
    Options reset_options;
    temp_options.tab_font_family =
            reset_options.tab_font_family;
    temp_options.tab_font_size =
            reset_options.tab_font_size;
    temp_options.tab_font_weight =
            reset_options.tab_font_weight;
    temp_options.tab_font_italic =
            reset_options.tab_font_italic;
}

void AppOptions::on_plainTextFontResetPushButton_clicked()
{
    Options reset_options;
    temp_options.plain_text_font_family =
            reset_options.plain_text_font_family;
    temp_options.plain_text_font_size =
            reset_options.plain_text_font_size;
    temp_options.plain_text_font_weight =
            reset_options.plain_text_font_weight;
    temp_options.plain_text_font_italic =
            reset_options.plain_text_font_italic;
}

void AppOptions::on_alertsFontResetPushButton_clicked()
{
    Options reset_options;
    temp_options.alerts_plain_text_font_family =
            reset_options.alerts_plain_text_font_family;
    temp_options.alerts_plain_text_font_size =
            reset_options.alerts_plain_text_font_size;
    temp_options.alerts_plain_text_font_weight =
            reset_options.alerts_plain_text_font_weight;
    temp_options.alerts_plain_text_font_italic =
            reset_options.alerts_plain_text_font_italic;
}
