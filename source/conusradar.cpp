#include "conusradar.h"
#include "ui_conusradar.h"
#include "appoptions.h"

extern Options options;         // access for the entire class

ConusRadar::ConusRadar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConusRadar)
{
    ui->setupUi(this);
    downloader = new Downloader();
    conusLabel = new QLabel();
    conusMovie = new QMovie();
    conusBuffer = new QBuffer();
    conus_radar_enabled = false;
    use_initial_conus_values = true;
    default_conus_size = QSize(options.conus_default_width,
                               options.conus_default_height);
    conus_h_scroll_bar = ui->scrollArea->horizontalScrollBar();
    conus_v_scroll_bar = ui->scrollArea->verticalScrollBar();
    zoom_factor = CONUSZOOMDEFAULT;
    pause_icon = QIcon(":/icons/weather-pause-300px.png");
    play_icon = QIcon(":/icons/weather-play-300px.png");
    connect(downloader, downloader->downloadRetrieved,
            this, processDownload);
    connect(conusMovie, conusMovie->frameChanged,
            this, updateSceneSlider);
    connect(ui->sceneSlider, ui->sceneSlider->valueChanged,
            this, updateScene);
}

ConusRadar::~ConusRadar()
{
    saveInitialScrollBarData();
    delete ui;
    delete downloader;
    delete conusLabel;
    delete conusMovie;
    delete conusBuffer;
}

bool ConusRadar::conusRadarEnabled(bool enable)
{
    if(conus_radar_enabled) return conus_radar_enabled;
    conus_radar_enabled = enable;
    // disable all input so it won't mess up the initial loading.
    ui->sceneSlider->setEnabled(false);
    ui->playPausePushButton->setEnabled(false);
    ui->zoomInPushButton->setEnabled(false);
    ui->zoomOutPushButton->setEnabled(false);
    ui->refreshPushButton->setEnabled(false);
    // load the "wait while loading" movie.
    conusMovie->setFileName(
                ":/gifs/weather-supercell.gif");
    // The following must be done prior to activation.
    ui->scrollArea->setWidget(conusLabel);
    conusLabel->setMovie(conusMovie);
    conusLabel->setScaledContents(true);
    conusMovie->start();
    refreshConusRadar();
    return conus_radar_enabled;
}

void ConusRadar::saveInitialScrollBarData()
{
    qreal val, max;
    options.conus_default_width_step =
            conus_h_scroll_bar->pageStep();
    options.conus_default_height_step =
            conus_v_scroll_bar->pageStep();
    val = conus_h_scroll_bar->value();
    max = conus_h_scroll_bar->maximum();
    if(max <= 0) max = 1;
    options.conus_h_scroll_ratio = val / max;
    val = conus_v_scroll_bar->value();
    max = conus_v_scroll_bar->maximum();
    if(max <= 0) max = 1;
    options.conus_v_scroll_ratio = val / max;
}

void ConusRadar::setInitialScrollBarData()
{
    conus_h_scroll_bar->setMaximum(options.conus_default_width);
    conus_v_scroll_bar->setMaximum(options.conus_default_width);
    conus_h_scroll_bar->setValue(options.conus_h_scroll_ratio *
                            (options.conus_default_width -
                             options.conus_default_width_step));
    conus_v_scroll_bar->setValue(options.conus_v_scroll_ratio *
                            (options.conus_default_height -
                             options.conus_default_height_step));
}

void ConusRadar::refreshConusRadar()
{
    if(!downloader->busy()) {
    downloader->requestDownload(CONUSRADAR);
    }
}

void ConusRadar::processDownload(ReturnData data)
{
    // stop, close, and reset everything.
    conusMovie->stop();
    conusBuffer->close();
    conusBuffer->setData(data.data_requested);
    conusMovie->setCacheMode(QMovie::CacheAll);
    conusMovie->setDevice(conusBuffer);
    conusLabel->setScaledContents(false);
    conusLabel->setMovie(conusMovie);

    // restart the movie.
    conusMovie->start();
    setupSceneSlider();

    // reset the icons.
    ui->playPausePushButton->setIcon(pause_icon);
    ui->playPausePushButton->setEnabled(true);
    ui->refreshPushButton->setEnabled(true);

    // enable/disable zoom in
    if(zoom_factor < CONUSZOOMMAX)
        ui->zoomInPushButton->setEnabled(true);
    else ui->zoomInPushButton->setEnabled(false);

    //  enable/disable zoom out
    if(zoom_factor > CONUSZOOMMIN)
        ui->zoomOutPushButton->setEnabled(true);
    else ui->zoomOutPushButton->setEnabled(false);

    // if this is the first load, use the initial scroll values.
    if(use_initial_conus_values) {
        use_initial_conus_values = false;
        setInitialScrollBarData();
    }
}

void ConusRadar::setupSceneSlider()
{
    bool has_frames = (conusMovie->frameCount() > 0);
    if(has_frames)
        ui->sceneSlider->setMaximum(
                    conusMovie->frameCount() - 1);
    ui->sceneSlider->setEnabled(has_frames);
}

void ConusRadar::updateSceneSlider()
{
    if(!ui->sceneSlider->isEnabled()) return;
    if(conusMovie->frameCount() <= 0) return;
    ui->sceneSlider->setValue(conusMovie->currentFrameNumber());
}

void ConusRadar::updateScene(qint16 number)
{
    conusMovie->jumpToFrame(number);
}

void ConusRadar::nextFrame()
{
    bool success = conusMovie->jumpToNextFrame();
    if(!success) conusMovie->jumpToFrame(0);
}

void ConusRadar::zoomRadar()
{
    /*
     * We are going to zoom the radar either in or out.
     * We can probe the current conus scroll bars to get the
     * necessary information, but we can't probe them again
     * to see how they change. That means that we have to make
     * a few assumptions:
     *
     * The pageStep usually is about the width of the display
     * area for the movie.
     *
     * We need to make sure to watch for the point when
     * maximum == 0. Since we calculate the ratio by dividing
     * the current value by the maximum (val / max) then we
     * will get a nasty divide by zero error if that happens.
     * To compensate, we check to make sure when max <= 0, we
     * change it to max = 1. With that done, val will still be
     * zero so we get the answer we are looking for without
     * the divide by zero problem.
     *
     * I am going to assume that the pageStep
     * is more-or-less constant. I will be treating it as such
     * unless I find a reason not to.
     */
    QSize current_size;
    qreal height, width;
    qreal h_step, v_step;
    qreal h_position, v_position, val, max;

    // first: calculate the horizontal position as a ratio.
    val = conus_h_scroll_bar->value();
    max = conus_h_scroll_bar->maximum();
    if(max <= 0) max = 1;
    h_step = conus_h_scroll_bar->pageStep();
    h_position = val / max;

    // second: calculate the vertical position as a ratio.
    val = conus_v_scroll_bar->value();
    max = conus_v_scroll_bar->maximum();
    if(max <= 0) max = 1;
    v_step = conus_v_scroll_bar->pageStep();
    v_position = val / max;

    // third: calculate the new zoom and set it.
    height = zoom_factor * default_conus_size.rheight();
    width = zoom_factor * default_conus_size.rwidth();
    current_size = QSize(width, height);
    conusMovie->setScaledSize(current_size);

    // forth: calculate the new horizontal maximum and set it.
    // (x/newMax) = h_position
    // x = h_position * newMax
    // newMax = width - h_step, with h_step constant.
    max = width - h_step;
    conus_h_scroll_bar->setMaximum(max);

    // fifth: set the new horizontal scroll bar position.
    conus_h_scroll_bar->setValue(h_position * max);

    // sixth: calculate the new vertical maximum and set it.
    max = height - v_step;
    conus_v_scroll_bar->setMaximum(max);

    // seventh: set the new vertical scroll bar position.
    conus_v_scroll_bar->setValue(v_position * max);

    // eighth: kluge -- manually go to the next frame so
    // that the display will update to the new settings.
    nextFrame();
}

void ConusRadar::on_playPausePushButton_clicked()
{
    qint16 state = conusMovie->state();
    if(state == QMovie::Paused) {
        conusMovie->setPaused(false);
        ui->playPausePushButton->setIcon(pause_icon);
    }
    else {
        conusMovie->setPaused(true);
        ui->playPausePushButton->setIcon(play_icon);
    }
}

void ConusRadar::on_zoomInPushButton_clicked()
{
    zoom_factor += CONUSZOOMSTEP;
    if(zoom_factor > CONUSZOOMMIN)
        ui->zoomOutPushButton->setEnabled(true);
    if(zoom_factor <= CONUSZOOMMAX)
        ui->zoomInPushButton->setEnabled(false);
    zoomRadar();
}

void ConusRadar::on_zoomOutPushButton_clicked()
{
    zoom_factor -= CONUSZOOMSTEP;
    if(zoom_factor <= CONUSZOOMMIN)
        ui->zoomOutPushButton->setEnabled(false);
    if(zoom_factor < CONUSZOOMMAX)
        ui->zoomInPushButton->setEnabled(true);
    zoomRadar();
}

void ConusRadar::on_refreshPushButton_clicked()
{
    // It's too easy to overload the graphics if I don't
    // reset the zoom prior to refreshing the radar.
    // I tried to do it during the update process, but
    // it didn't work.
    zoom_factor = CONUSZOOMDEFAULT;
    zoomRadar();
    refreshConusRadar();
}
