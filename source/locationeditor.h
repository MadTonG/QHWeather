#ifndef LOCATIONEDITOR_H
#define LOCATIONEDITOR_H

#include <QDialog>
#include <QFont>
#include <QString>
#include <QStringBuilder>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include "downloader.h"
#include "appoptions.h"

// Definitions for the process_requested.
#define ADDLOCATION 1
#define EDITLOCATION 2

/*
 * Mapquest resources
 * The url we create will be passed as a get request.
 * The order of the url should be as follows:
 *
 * MAPQUESTRESOURCE
 * % MAPQUESTKEY
 * % MAPQUESTMAXRESULTS
 * % MAPQUESTFORMAT
 * % MAPQUESTLOCATION
 * % [the location string that the user entered]
 *
 * Other options could be included prior to the MAPQUESTLOCATION
 * if we decide that we need them in the future.
 *
 * The results will be a QByteArray that contains a json formated
 * array. That array should be decoded according to the
 * Mapquest api found at:
 * developer.mapquest.com/documentation/geocoding-api/address/get/
 */

const QString MAPQUESTRESOURCE =
        "http://www.mapquestapi.com/geocoding/v1/address";
const QString MAPQUESTKEY =
        "?key=AXtUjv1r0gJhbOCbUEByXoT6JMbuCBRr";
const QString MAPQUESTMAXRESULTS = "&maxResults=1";
const QString MAPQUESTFORMAT = "&outFormat=json";
const QString MAPQUESTLOCATION = "&location=";

struct ProcessedLocation {
    ForecastLocations location;
    qint32 row;
    qint16 process_requested;
};

namespace Ui {
class LocationEditor;
}

class LocationEditor : public QDialog
{
    Q_OBJECT

public:
    explicit LocationEditor(QWidget *parent = 0);
    ~LocationEditor();
    void addLocation();
    void editLocation(ForecastLocations location, qint32 row);

signals:
    void locationData(ProcessedLocation);

private slots:
    void on_searchPushButton_clicked();
    void on_okayPushButton_clicked();
    void on_cancelPushButton_clicked();

private:
    Ui::LocationEditor *ui;
    Downloader *downloader;
    ProcessedLocation processed_location;

    void updateUserInputFields();
    void retrieveUserInputFields();
    void processDownload(ReturnData data);
};

#endif // LOCATIONEDITOR_H
