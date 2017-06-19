#include "locationeditor.h"
#include "ui_locationeditor.h"

LocationEditor::LocationEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocationEditor),
    downloader(new Downloader)
{
    ui->setupUi(this);
    connect(downloader, downloader->downloadRetrieved,
            this, processDownload);
}

LocationEditor::~LocationEditor()
{
    delete ui;
    delete downloader;
}

void LocationEditor::addLocation()
{
    processed_location = {};
    processed_location.process_requested = ADDLOCATION;
    updateUserInputFields();
}

void LocationEditor::editLocation(ForecastLocations location,
                                  qint32 row)
{
    processed_location = {};
    processed_location.location = location;
    processed_location.row = row;
    processed_location.process_requested = EDITLOCATION;
    updateUserInputFields();
}

void LocationEditor::updateUserInputFields()
{
    QString lat, lon;
    /*
     * Four decimal places is all that is allowed by the NWS.
     * We will save whatever the user inputs, but we will only
     * display four decimal places.
     */
    lat = QString::number(processed_location.location.latitude,
                          'f', 4);
    lon = QString::number(processed_location.location.longitude,
                          'f', 4);
    ui->locationNameLineEdit->setText(
                processed_location.location.location_name);
    ui->latitudeLineEdit->setText(lat);
    ui->longitudeLineEdit->setText(lon);
    ui->defaultLocationCheckBox->setChecked(
                processed_location.location.default_location);
}

void LocationEditor::retrieveUserInputFields()
{
    processed_location.location.location_name =
            ui->locationNameLineEdit->text();
    processed_location.location.latitude =
            ui->latitudeLineEdit->text().toDouble();
    processed_location.location.longitude =
            ui->longitudeLineEdit->text().toDouble();
    processed_location.location.default_location =
            ui->defaultLocationCheckBox->isChecked();
}

void LocationEditor::processDownload(ReturnData data)
{
    qreal display_lat, display_lon;
    qreal lat, lon;
    QString admin_area_6, admin_area_5, admin_area_4;
    QString admin_area_3, admin_area_2, admin_area_1;
    QString postal_code;
    /*
     * This method will be used as a slot to receive the data
     * that will be returned by the "download" module.
     *
     * It will parse the json data returned from Mapquest
     * and put the values in the appropriate text edit boxes.
     */

    // first step: clear the input box
    // we have cleared this when the search button was pressed,
    // but it doesn't hurt to leave this line in place.
    ui->locationStringLineEdit->setText("");

    // second step: get arrays and objects that we need.
    QJsonDocument mapquest_document =
            QJsonDocument::fromJson(data.data_requested);
    QJsonObject mapquest_object = mapquest_document.object();
    QJsonObject info_object =
            mapquest_object.value("info").toObject();
    QJsonArray results_array =
            mapquest_object.value("results").toArray();
    QJsonObject results_object_1 =
            results_array[0].toObject();
    QJsonArray locations_array =
            results_object_1.value("locations").toArray();
    QJsonObject locations_object_1 =
            locations_array[0].toObject();
    QJsonObject lat_lng =
            locations_object_1.value("latLng").toObject();
    QJsonObject display_lat_lng =
            locations_object_1.value("displayLatLng").toObject();

    // third step: update the latitude and longitude information.
    lat = lat_lng.value("lat").toDouble();
    lon = lat_lng.value("lng").toDouble();
    display_lat = display_lat_lng.value("lat").toDouble();
    display_lon = display_lat_lng.value("lng").toDouble();
    if(display_lat == 0.0 || display_lon == 0) {
        ui->latitudeLineEdit->setText(
                    QString::number(lat, 'f', 4));
        ui->longitudeLineEdit->setText(
                    QString::number(lon, 'f', 4));
    }
    else {
        ui->latitudeLineEdit->setText(
                    QString::number(display_lat, 'f', 4));
        ui->longitudeLineEdit->setText(
                    QString::number(display_lon, 'f', 4));
    }

    // fourth step: provide some kind of name string
    admin_area_6 =
            locations_object_1.value("adminArea6").toString();
    admin_area_5 =
            locations_object_1.value("adminArea5").toString();
    admin_area_4 =
            locations_object_1.value("adminArea4").toString();
    admin_area_3 =
            locations_object_1.value("adminArea3").toString();
    admin_area_2 =
            locations_object_1.value("adminArea2").toString();
    admin_area_1 =
            locations_object_1.value("adminArea1").toString();
    postal_code =
            locations_object_1.value("postalCode").toString();
    if(admin_area_6.length() > 0)
            ui->locationNameLineEdit->setText(admin_area_6);
    else if(admin_area_5.length() > 0)
            ui->locationNameLineEdit->setText(admin_area_5);
    else if(admin_area_4.length() > 0)
            ui->locationNameLineEdit->setText(admin_area_4);
    else if(admin_area_3.length() > 0)
            ui->locationNameLineEdit->setText(admin_area_3);
    else if(admin_area_2.length() > 0)
            ui->locationNameLineEdit->setText(admin_area_2);
    else if(admin_area_1.length() > 0)
            ui->locationNameLineEdit->setText(admin_area_1);
    else if(postal_code.length() > 0)
            ui->locationNameLineEdit->setText(postal_code);
    else ui->locationNameLineEdit->setText("");
}

void LocationEditor::on_searchPushButton_clicked()
{
    QString url, user_input;
    user_input = ui->locationStringLineEdit->text();
    ui->locationStringLineEdit->setText("");
    if(user_input.length() > 0) {
        url = MAPQUESTRESOURCE
                % MAPQUESTKEY
                % MAPQUESTMAXRESULTS
                % MAPQUESTFORMAT
                % MAPQUESTLOCATION
                % user_input;
        downloader->requestDownload(url);
    }
    else {
        extern Options options;
        QFont basic_font;
        basic_font = QFont(options.basic_font_family,
                           options.basic_font_size);
        QString msg, note;
        msg = "Please enter a <b>street address</b>, ";
        msg += "<b>city</b>, <b>state</b> ";
        msg += "and/or <b>zipcode</b> in any combination.";
        note = "The more information you enter, ";
        note += "the more accurate the results will be.";
        QMessageBox message_box;
        message_box.setIcon(QMessageBox::Information);
        message_box.setFont(basic_font);
        message_box.setText(msg);
        message_box.setInformativeText(note);
        message_box.exec();
    }
}

void LocationEditor::on_okayPushButton_clicked()
{
    retrieveUserInputFields();
    emit locationData(processed_location);
    this->accept();
}

void LocationEditor::on_cancelPushButton_clicked()
{
    this->reject();
}
