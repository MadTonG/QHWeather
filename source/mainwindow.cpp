#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    alerts_window(new AlertsWindow),
    app_options(new AppOptions),
    location_editor(new LocationEditor)
{
    extern Options options;
    ui->setupUi(this);
    /*
     * We must load the options or we will get the defaults.
     * If there are no options, the loadProgramOptions method
     * should leave the defaults in place.
     */
    app_options->loadProgramOptions();
    loadUiFonts();
    loadLocations();
    connect(ui->availableLocationsListWidget,
            ui->availableLocationsListWidget->currentRowChanged,
            this, updateLocation);
    connect(ui->availableLocationsListWidget,
            ui->availableLocationsListWidget->itemDoubleClicked,
            this, updateAndRefreshLocation);
    connect(location_editor, location_editor->locationData,
            this, processLocation);
    connect(app_options, app_options->accepted,
            this, loadUiFonts);
    for(int x=0; x<NUM_DOWNLOADERS; x++) {
        std::unique_ptr<Downloader> connection(new Downloader());
        connect(connection.get(),
                connection.get()->downloadRetrieved,
                this,
                processDownload);
        vec.push_back(std::move(connection));
    }
    clear_icon = QIcon(":/icons/weather-clear-300px.png");
    alert_icon = QIcon(":/icons/weather-severe-alert-300px.png");
    number_active_alerts = 0;
    endpoint_urls = {};
    if(options.load_on_start) loadForecast();
    else userInputEnabled(true);
}

MainWindow::~MainWindow()
{
    app_options->saveProgramOptions();
    delete ui;
    delete alerts_window;
    delete app_options;
    delete location_editor;
}

void MainWindow::loadUiFonts()
{
    extern Options options;
    basic_font = QFont(options.basic_font_family,
                       options.basic_font_size,
                       options.basic_font_weight,
                       options.basic_font_italic);
    ui->currentForecastLocationLabel->setFont(basic_font);
    ui->availableLocationsListWidget->setFont(basic_font);
    ui->detailedForecastTextEdit->setFont(basic_font);
    ui->detailedForecastTextEdit->setReadOnly(true);
    plain_text_font = QFont(options.plain_text_font_family,
                            options.plain_text_font_size,
                            options.plain_text_font_weight,
                            options.plain_text_font_italic);
    ui->afdPlainTextEdit->setFont(plain_text_font);
    ui->afdPlainTextEdit->setReadOnly(true);
    ui->hwoPlainTextEdit->setFont(plain_text_font);
    ui->hwoPlainTextEdit->setReadOnly(true);
    tab_font = QFont(options.tab_font_family,
                     options.tab_font_size,
                     options.tab_font_weight,
                     options.tab_font_italic);
    ui->tabWidget->setFont(tab_font);
}

void MainWindow::loadLocations()
{
    extern std::vector<ForecastLocations> forecast_locations;
    unsigned int y;
    ui->availableLocationsListWidget->clear();
    for(y=0; y<forecast_locations.size(); ++y) {
        new QListWidgetItem(forecast_locations[y].location_name,
                            ui->availableLocationsListWidget);
        if(forecast_locations[y].default_location) {
            ui->availableLocationsListWidget->setCurrentRow(y);
            updateLocation(y);
        }
    }
}

void MainWindow::updateLocation(int index)
{
    extern Options options;
    extern std::vector<ForecastLocations> forecast_locations;
    /*
     * Note: the index can become out of range when the list
     * is deleted. If that happens, we just set everyting back
     * to blank and wait for a selection to be made.
     */
    if(index < 0 || index > forecast_locations.size()) {
        options.location_name = "";
        options.latitude = 0.0;
        options.longitude = 0.0;
        return;
    }
    options.location_name =
            forecast_locations[index].location_name;
    options.latitude = forecast_locations[index].latitude;
    options.longitude = forecast_locations[index].longitude;
}

void MainWindow::updateAndRefreshLocation()
{
    /*
     * The itemDoubleClicked signal that is connected to this
     * method can send information. We, however, have no use
     * for it. Because we have no use for it, we aren't
     * including it in the method. If the program structure
     * changes at some point in the future, we might have to
     * start receiving the "QListWidgetItem *item" that is
     * provided by the itemDoubleClicked signal.
     */
    updateLocation(
                ui->availableLocationsListWidget->currentRow());
    loadForecast();
}

void MainWindow::processLocation(ProcessedLocation loc)
{
    extern std::vector<ForecastLocations> forecast_locations;
    if(loc.process_requested == ADDLOCATION)
        forecast_locations.push_back(loc.location);
    if(loc.process_requested == EDITLOCATION)
        forecast_locations[loc.row] = loc.location;
    loadLocations();
}

void MainWindow::processDownload(ReturnData data)
{
    extern Options options;
    if(data.caller_type == TYPE_POINTS) {
        if(options.debug) {
            QFile file;
            file.setFileName("NWS_debug.txt");
            file.open(QFile::WriteOnly);
            file.write(data.data_requested);
            file.close();
        }
        endpoints.extractUrlsFromPoints(
                    data.data_requested,
                    endpoint_urls);
        delayed_request = {};
        delayed_request.caller_type = TYPE_PRODUCTS;
        delayed_request.url_string =
                endpoint_urls.products_locations_types;
        request_queue.push(delayed_request);
    }
    if(data.caller_type == TYPE_FORECAST) {
        endpoints.parsePointsForecast(data.data_requested,
                    endpoint_urls,
                    points_forecast);
        QString detailed_forecast;
        pointsForecastToHtml(detailed_forecast);
        ui->detailedForecastTextEdit->document()->clear();
        ui->detailedForecastTextEdit->setHtml(detailed_forecast);
        if(options.load_icons) queueIcons();
    }
    if(data.caller_type == TYPE_PRODUCTS) {
        endpoints.createUrlsFromProducts(data.data_requested,
                                         endpoint_urls);
        if(endpoint_urls.products_types_AFD_locations.length()
                                                    != 0) {
            delayed_request = {};
            delayed_request.caller_type = TYPE_AFD;
            delayed_request.url_string =
                    endpoint_urls.products_types_AFD_locations;
            request_queue.push(delayed_request);
        }
        if(endpoint_urls.products_types_HWO_locations.length()
                                                    != 0 ) {
            delayed_request = {};
            delayed_request.caller_type = TYPE_HWO;
            delayed_request.url_string =
                    endpoint_urls.products_types_HWO_locations;
            request_queue.push(delayed_request);
        }
    }
    if(data.caller_type == TYPE_AFD) {
        endpoints.extractLatestAfdUrl(data.data_requested,
                                   endpoint_urls);
        delayed_request = {};
        delayed_request.caller_type = TYPE_LATEST_ADF;
        delayed_request.url_string =
                endpoint_urls.latest_AFD;
        request_queue.push(delayed_request);
    }
    if(data.caller_type == TYPE_LATEST_ADF) {
        QString latest_afd = endpoints.extractLatestAfd(
                    data.data_requested);
        ui->afdPlainTextEdit->setPlainText(latest_afd);
    }
    if(data.caller_type == TYPE_HWO) {
        endpoints.extractLatestHwoUrl(data.data_requested,
                                      endpoint_urls);
        delayed_request = {};
        delayed_request.caller_type = TYPE_LATEST_HWO;
        delayed_request.url_string =
                endpoint_urls.latest_HWO;
        request_queue.push(delayed_request);
    }
    if(data.caller_type == TYPE_LATEST_HWO) {
        QString latest_hwo = endpoints.extractLatestHwo(
                    data.data_requested);
        ui->hwoPlainTextEdit->setPlainText(latest_hwo);
    }
    if(data.caller_type == TYPE_STATIONS) {
    }
    if(data.caller_type == TYPE_ALERTS) {
        number_active_alerts = endpoints.parseActiveAlerts(
                    data.data_requested,
                    active_alerts);
    }
    if(data.caller_type == TYPE_ICON) {
        QString icon_url;
        QTextDocument *detailed_document;
        detailed_document =
                ui->detailedForecastTextEdit->document();
        QTextCursor detailed_cursor(detailed_document);
        qint16 block_number = data.caller_index;
        block_number *= 2;
        detailed_cursor.movePosition(
                    QTextCursor::NextBlock,
                    QTextCursor::MoveAnchor,
                    block_number);
        icon_url = "nws://icon_"
                % QString::number(data.caller_index, 'f', 0)
                % ".png";
        detailed_document->addResource(
                    QTextDocument::ImageResource,
                    icon_url,
                    data.data_requested);
        QTextImageFormat icon_format;
        icon_format.setName(icon_url);
        detailed_cursor.insertImage(icon_format);
        detailed_cursor.insertText("    ");
    }
    if(updateRequests() == 0) {
        // If no more requests are waiting
        userInputEnabled(true);
        QTime time =
                QDateTime::fromString(endpoint_urls.updatedTime,
                            Qt::ISODate).toLocalTime().time();
        QString city_state_time;
        city_state_time =
            "<b>National Weather Service</b> forecast for:<br/>"
                % endpoint_urls.city
                % ", "
                % endpoint_urls.state;
        if(time.isValid()) {
            city_state_time += " at: "
                    % time.toString("h:mm ap");
        }
        ui->currentForecastLocationLabel->setText(
                    city_state_time);
    }
}

qint16 MainWindow::updateRequests()
{
    qint16 active_requests = 0;
    for(int x=0; x<NUM_DOWNLOADERS; ++x) {
        if(!vec[x]->busy()) {
            if(!request_queue.empty()) {
                vec[x]->requestDownload(request_queue.front());
                request_queue.pop();
                // if we add a request, we need to count it.
                active_requests++;
            }
        }
        // if the vec[x] was busy, then we need to count it.
        else active_requests++;
    }
    return active_requests;
}

void MainWindow::loadForecast()
{
    extern Options options;
    bool success;
    // disable user input until update is finished
    userInputEnabled(false);
    // clear any data from previous update before starting.
    points_forecast.clear();
    active_alerts.clear();
    number_active_alerts = 0;
    endpoint_urls = {};
    // clear user interface
    ui->detailedForecastTextEdit->clear();
    ui->hwoPlainTextEdit->clear();
    ui->afdPlainTextEdit->clear();
    success = endpoints.setPoint(options.latitude,
                                 options.longitude,
                                 endpoint_urls);
    if(!success) return;

    // Update the currentForecastLocationLabel text to
    // display "Updating..."
    ui->currentForecastLocationLabel->setText("Updating...");

    // set the default headers for the download
    network_request.setRawHeader("Accept", NWS_ACCEPT);
    network_request.setRawHeader("User-Agent", NWS_USER_AGENT);
    network_request.setAttribute(
                QNetworkRequest::FollowRedirectsAttribute,
                true);
    for(int x=0; x<NUM_DOWNLOADERS; ++x)
        vec[x]->setAlternateRequestInfo(network_request);
    delayed_request = {};
    delayed_request.caller_type = TYPE_POINTS;
    delayed_request.url_string = endpoint_urls.points_base;
    request_queue.push(delayed_request);
    delayed_request.caller_type = TYPE_FORECAST;
    delayed_request.url_string = endpoint_urls.points_forecast;
    request_queue.push(delayed_request);
    delayed_request.caller_type = TYPE_STATIONS;
    delayed_request.url_string = endpoint_urls.points_stations;
    request_queue.push(delayed_request);
    delayed_request.caller_type = TYPE_ALERTS;
    delayed_request.url_string =
            endpoint_urls.alerts_point_active;
    request_queue.push(delayed_request);
    updateRequests();
}

void MainWindow::pointsForecastToHtml(QString &detailed_forecast)
{
    extern Options options;
    QString detailed_cast = "";
    qint16 len_points_forecast = points_forecast.size();
    for(int x=0; x<len_points_forecast; ++x) {
        detailed_cast += "<p style=\"text-align: "
                % options.name_align % "; font-size: "
                % options.name_size % "px" % "; color: "
                % options.name_color % "; line-height: "
                % options.name_line_height % "px"
                % " ; vertical-align: middle"
                % "\">"
                % points_forecast[x].name
                % "</p>";
        detailed_cast += "<p style=\"text-align: "
                % options.detailed_align % "; font-size: "
                % options.detailed_size % "px" % "; color: "
                % options.detailed_color % "\">"
                % points_forecast[x].detailedForecast
                % "</p>";
    }
    detailed_forecast = detailed_cast;
}

void MainWindow::queueIcons()
{
    qint16 number_icons = points_forecast.size();
    for(qint16 x=0; x<number_icons; ++x) {
        delayed_request = {};
        delayed_request.caller_type = TYPE_ICON;
        delayed_request.caller_index = x;
        delayed_request.url_string =
                points_forecast[x].icon;
        request_queue.push(delayed_request);

    }
}

void MainWindow::userInputEnabled(bool updated_state)
{
    forecast_updating = !updated_state;
    ui->reloadForecastPushButton->setEnabled(updated_state);
    if(number_active_alerts > 0)
            ui->warningPushButton->setIcon(alert_icon);
    else ui->warningPushButton->setIcon(clear_icon);
    ui->warningPushButton->setEnabled(updated_state);

}

void MainWindow::on_reloadForecastPushButton_clicked()
{
    endpoint_urls = {};
    loadForecast();
}

void MainWindow::on_radarPushButton_clicked()
{
    ConusRadar conus_radar;
    conus_radar.setModal(true);
    conus_radar.conusRadarEnabled(true);
    conus_radar.exec();
}

void MainWindow::on_warningPushButton_clicked()
{
    /*
     * With this method, we are going to show the window
     * BEFORE we call the window's displayAlerts method.
     * The reason that we do that is to make sure that the
     * scroll will always be at the top of the window.
     * And yes, this is anothe kluge.
     */
    alerts_window->setModal(false);
    alerts_window->show();
    alerts_window->displayAlerts(active_alerts);
    // alerts_window->show();
}

void MainWindow::on_ridgePushButton_clicked()
{
    RidgeRadarLite ridge_radar_lite;
    ridge_radar_lite.setModal(true);
    ridge_radar_lite.receiveRidgeLocations(endpoint_urls);
    ridge_radar_lite.exec();
}

void MainWindow::on_settingsPushButton_clicked()
{
    app_options->setModal(false);
    app_options->initiateEditor();
    app_options->show();
}

void MainWindow::on_addLocationPushButton_clicked()
{
    location_editor->setModal(false);
    location_editor->addLocation();
    location_editor->show();
}

void MainWindow::on_editLocationPushButton_clicked()
{
    extern std::vector<ForecastLocations> forecast_locations;
    qint32 row = ui->availableLocationsListWidget->currentRow();
    if(row < 0 || row > forecast_locations.size()) return;
    location_editor->setModal(false);
    location_editor->editLocation(forecast_locations[row], row);
    location_editor->show();
}

void MainWindow::on_deleteLocationPushButton_clicked()
{
    extern std::vector<ForecastLocations> forecast_locations;
    qint32 row = ui->availableLocationsListWidget->currentRow();
    if(row < 0 || row > forecast_locations.size()) return;
    QString message = "<b>Are you sure</b> want to delete \""
            % forecast_locations[row].location_name
            % "\"";
    QMessageBox message_box;
    message_box.setFont(basic_font);
    message_box.setIcon(QMessageBox::Warning);
    message_box.setText(message);
    message_box.setInformativeText(
                "This action can not be undone.");
    message_box.setStandardButtons(
                QMessageBox::No | QMessageBox::Yes);
    message_box.setDefaultButton(QMessageBox::No);
    int answer = message_box.exec();
    if(answer == QMessageBox::Yes) {
        forecast_locations.erase(
                    forecast_locations.cbegin()+row);
        loadLocations();
    }
}
