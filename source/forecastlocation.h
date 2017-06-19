#ifndef FORECASTLOCATION_H
#define FORECASTLOCATION_H

#include <QDialog>

namespace Ui {
class ForecastLocation;
}

class ForecastLocation : public QDialog
{
    Q_OBJECT

public:
    explicit ForecastLocation(QWidget *parent = 0);
    ~ForecastLocation();
    void addLocation()

private:
    Ui::ForecastLocation *ui;
};

#endif // FORECASTLOCATION_H
