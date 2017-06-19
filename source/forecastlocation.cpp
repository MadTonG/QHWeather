#include "forecastlocation.h"
#include "ui_forecastlocation.h"

ForecastLocation::ForecastLocation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ForecastLocation)
{
    ui->setupUi(this);
}

ForecastLocation::~ForecastLocation()
{
    delete ui;
}
