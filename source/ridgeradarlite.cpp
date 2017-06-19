#include "ridgeradarlite.h"
#include "ui_ridgeradarlite.h"

RidgeRadarLite::RidgeRadarLite(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RidgeRadarLite)
{
    extern Options options;
    ui->setupUi(this);
    downloader = new Downloader();
    ridgeLabel = new QLabel();
    ridgeMovie = new QMovie();
    ridgeBuffer = new QBuffer();
    basic_font = QFont(options.basic_font_family,
                        options.basic_font_size);
    selected_ridge_radar_url = "";
    radar_station = "";
    ui->stateLabel->setFont(basic_font);
    ui->radarLabel->setFont(basic_font);
    ui->stateComboBox->setFont(basic_font);
    ui->radarComboBox->setFont(basic_font);
    loadRadarLocations();
    ridge_radar_lite_enabled = false;
    zoom_factor = RIDGEZOOMDEFAULT;
    ridge_default_size = QSize(options.ridge_default_width,
                               options.ridge_default_height);
    ridge_h_scroll_bar = ui->scrollArea->horizontalScrollBar();
    ridge_v_scroll_bar = ui->scrollArea->verticalScrollBar();
    pause_icon = QIcon(":/icons/weather-pause-300px.png");
    play_icon = QIcon(":/icons/weather-play-300px.png");
    connect(downloader, downloader->downloadRetrieved,
            this, processDownload);
    connect(ui->stateComboBox,
            ui->stateComboBox->currentTextChanged,
            this, loadRadarComboBox);
    connect(ui->radarComboBox,
            SIGNAL(currentIndexChanged(int)),
            this, SLOT(ridgeRadarSelected(int)));
    connect(ridgeMovie, ridgeMovie->frameChanged,
            this, updateSceneSlider);
    connect(ui->sceneSlider, ui->sceneSlider->valueChanged,
            this, updateScene);
}

RidgeRadarLite::~RidgeRadarLite()
{
    delete ui;
    delete downloader;
    delete ridgeLabel;
    delete ridgeMovie;
    delete ridgeBuffer;
}

void RidgeRadarLite::receiveRidgeLocations(EndpointUrls &urls)
{
    /*
     * This is a kluge.
     * It has two purposes.
     * Initially, it provides the inforation necessary -- if
     * available -- to load the correct radar.
     * And two, it acts as the setRadarEnabled function in the
     * sense that it will block any further calls.
     */
    if(ridge_radar_lite_enabled) return;
    ridge_radar_lite_enabled = true;
    // If the radar hasn't previously been enabled, go through
    // the startup sequence:
    ui->sceneSlider->setEnabled(false);
    ui->playPausePushButton->setEnabled(false);
    ui->zoomInPushButton->setEnabled(false);
    ui->zoomOutPushButton->setEnabled(false);
    ui->refreshPushButton->setEnabled(false);

    // unlike the CONUS radar, there seems to be no need
    // to cache the contents. Why? Is it because of the size?
    ridgeMovie->setFileName(":/gifs/weather-supercell.gif");
    ridgeLabel->setMovie(ridgeMovie);
    ridgeLabel->setScaledContents(true);
    ui->scrollArea->setWidget(ridgeLabel);
    ridgeMovie->start();

    // save a copy of the urls for future use
    endpoint_urls = urls;
    radar_station = urls.radar_station;

    // If we have a state, try to set the state combo box and
    // refresh the ridge radar.
    if(endpoint_urls.state.length() > 0) {
        qint32 x = ui->stateComboBox->findText(urls.state);
        if(x >= 0) ui->stateComboBox->setCurrentIndex(x);
        refreshRidgeRadar();
    }
}

void RidgeRadarLite::refreshRidgeRadar()
{
    /* This method will not operate unless
     * there is a radar selected. As of yet, it doesn't verify
     * that the radar is valid.
     */
    if(radar_station == "") return;

    /*
     * Currently the only radar used is the N0R radar.
     * Future versions will add a way to select from all the
     * different radars that the National Weather Service offers.
     */
    if(!downloader->busy()) {
        selected_ridge_radar_url = N0R
                % radar_station
                % "_loop.gif";
        downloader->requestDownload(selected_ridge_radar_url);
    }
}

void RidgeRadarLite::processDownload(ReturnData data)
{
    // stop, close, and reset everything
    ridgeBuffer->close();
    ridgeMovie->stop();
    ridgeBuffer->setData(data.data_requested);
    ridgeMovie->setCacheMode(QMovie::CacheAll);
    ridgeMovie->setDevice(ridgeBuffer);
    ridgeLabel->setScaledContents(false);
    ridgeLabel->setMovie(ridgeMovie);

    // restart the movie
    ridgeMovie->start();
    setupSceneSlider();

    // enable icons
    ui->playPausePushButton->setIcon(pause_icon);
    ui->playPausePushButton->setEnabled(true);
    ui->refreshPushButton->setEnabled(true);

    // enable/disable zoom in
    if(zoom_factor < RIDGEZOOMMAX)
        ui->zoomInPushButton->setEnabled(true);
    else ui->zoomInPushButton->setEnabled(false);

    // enable/disable zoom out
    if(zoom_factor > RIDGEZOOMMIN)
        ui->zoomOutPushButton->setEnabled(true);
    else ui->zoomOutPushButton->setEnabled(false);
}

void RidgeRadarLite::setupSceneSlider()
{
    bool has_frames = (ridgeMovie->frameCount() > 0);
    if(has_frames)
        ui->sceneSlider->setMaximum(
                    ridgeMovie->frameCount() - 1);
    ui->sceneSlider->setEnabled(has_frames);
}

void RidgeRadarLite::updateSceneSlider()
{
    if(!ui->sceneSlider->isEnabled()) return;
    if(ridgeMovie->frameCount() <= 0) return;
    ui->sceneSlider->setValue(ridgeMovie->currentFrameNumber());
}

void RidgeRadarLite::updateScene(qint32 number)
{
    ridgeMovie->jumpToFrame(number);
}


void RidgeRadarLite::nextFrame()
{
    ridgeMovie->jumpToNextFrame();
    ridgeMovie->jumpToFrame(0);
}

void RidgeRadarLite::zoomRadar()
{
    qreal h_scroll_ratio;
    qreal v_scroll_ratio;
    qreal value, maximum, width, height, v_step, h_step;
    QSize current_size;

    // get location, maximum, and step of horizontal bar.
    value = ridge_h_scroll_bar->value();
    maximum = ridge_h_scroll_bar->maximum();
    h_step = ridge_h_scroll_bar->pageStep();

    // calculate the ratio
    if(maximum <= 0) maximum = 1;
    h_scroll_ratio = value / maximum;


    // get location, maximum, and step for vertical bar.
    value = ridge_v_scroll_bar->value();
    maximum = ridge_v_scroll_bar->maximum();
    v_step = ridge_v_scroll_bar->pageStep();

    // calculate the ratio.
    if(maximum <= 0) maximum = 1;
    v_scroll_ratio = value / maximum;

    // resize the movie
    width = zoom_factor * ridge_default_size.rwidth();
    height = zoom_factor * ridge_default_size.rheight();
    current_size.rheight() = height;
    current_size.rwidth() = width;
    ridgeMovie->setScaledSize(current_size);

    // set new horizontal maximum and position
    maximum = width - h_step;
    ridge_h_scroll_bar->setMaximum(maximum);
    ridge_h_scroll_bar->setSliderPosition(h_scroll_ratio *
                                          maximum);

    // set new vertical maximum and position
    maximum = height - v_step;
    ridge_v_scroll_bar->setMaximum(maximum);
    ridge_v_scroll_bar->setSliderPosition(v_scroll_ratio *
                                          maximum);

    // kluge to force the ridgeMovie to refresh
    nextFrame();
}

void RidgeRadarLite::loadRadarLocations()
{
    /*
     * This method loads the radar data from a file that is
     * compiled into the program resources. It is the first
     * step in making the radars available from the
     * various combo boxes.
     */
    QByteArray json_byte_array;
    QFile file;
    file.setFileName(":/json/weather-radars.json");
    file.open(QFile::ReadOnly);
    json_byte_array = file.readAll();
    file.close();
    radar_location_document =
            QJsonDocument::fromJson(json_byte_array);
    state_json_object = radar_location_document.object();
    loadStateComboBox();
}

void RidgeRadarLite::loadStateComboBox()
{
    /*
     * This is the second step of loading the state combo box.
     * It requires that the loadRadarLocations method was called
     * prior to its use. However, once the loadRadarLocations
     * method has been called, it can load the state combo box as
     * many times as necessary.
     */
    QJsonObject::iterator iterator;
    ui->stateComboBox->addItem("Select State", "XX");
    ui->stateComboBox->addItem("ALL", "XX");
    for(iterator = state_json_object.begin();
        iterator != state_json_object.end();
        ++iterator) {
        ui->stateComboBox->addItem(
                    iterator.key(), iterator.key());

    }
}

void RidgeRadarLite::loadRadarComboBox(QString text)
{
    /*
     * The text that this method receives should be the state
     * that was selected in the state combo box. Alternately,
     * it could also be "ALL" if the user so chose.
     *
     * This method loads the radar combo box. It requires that
     * the loadRadarLocations method has been called previously.
     * It will then change the available options depending
     * on what selection has been made in the state combo box.
     */
    QString state_site_combo = "";
    ui->radarComboBox->clear();
    if(text == "Select State") {

        // If the user selected the "Select State" option again,
        // do nothing.
    }
    else if(text == "ALL") {

        // The user wants to see all the radars.
        ui->radarComboBox->addItem("Select Site", "");
        QJsonObject::iterator state_iterator;
        for(state_iterator = state_json_object.begin();
            state_iterator != state_json_object.end();
            ++state_iterator) {
            QJsonArray current_array =
                    state_json_object
                    .value(state_iterator.key()).toArray();
            for(int x=0; x<current_array.size(); ++x) {
                QJsonObject site_object =
                        current_array[x].toObject();
                state_site_combo = state_iterator.key()
                        % " - "
                        % site_object.value("siteName")
                                            .toString();
                ui->radarComboBox->addItem(
                    state_site_combo,
                    site_object.value("identifier").toString());
            }
        }
    }
    else {

        // Here, a specific state should have been selected.
        // We will only load the radar sites for that state.
        ui->radarComboBox->addItem("Select Site", "");
        QJsonArray current_array =
                state_json_object.value(text).toArray();
        for(int x=0; x<current_array.size(); ++x) {
            QJsonObject site_object =
                    current_array[x].toObject();
            ui->radarComboBox->addItem(
                    site_object.value("siteName").toString(),
                    site_object.value("identifier").toString());
        }
    }
}

void RidgeRadarLite::ridgeRadarSelected(int index)
{
    // index 0 is the place where the direction text is located.
    // it doesn't contain any information.
    if(index > 0) {
        radar_station =
            ui->radarComboBox->itemData(index).toString();
        refreshRidgeRadar();
    }
}

void RidgeRadarLite::on_playPausePushButton_clicked()
{
    qint32 state = ridgeMovie->state();
    if(state == QMovie::Paused) {
        ridgeMovie->setPaused(false);
        ui->playPausePushButton->setIcon(pause_icon);
    }
    else {
        ridgeMovie->setPaused(true);
        ui->playPausePushButton->setIcon(play_icon);
    }

}

void RidgeRadarLite::on_zoomInPushButton_clicked()
{
    zoom_factor += RIDGEZOOMSTEP;
    zoomRadar();
    if(zoom_factor > RIDGEZOOMMIN)
        ui->zoomOutPushButton->setEnabled(true);
    if(zoom_factor >= RIDGEZOOMMAX)
        ui->zoomInPushButton->setEnabled(false);
}

void RidgeRadarLite::on_zoomOutPushButton_clicked()
{
    zoom_factor -= RIDGEZOOMSTEP;
    zoomRadar();
    if(zoom_factor <= RIDGEZOOMMIN)
        ui->zoomOutPushButton->setEnabled(false);
    if(zoom_factor < RIDGEZOOMMAX)
        ui->zoomInPushButton->setEnabled(true);
}

void RidgeRadarLite::on_refreshPushButton_clicked()
{
    refreshRidgeRadar();
}
