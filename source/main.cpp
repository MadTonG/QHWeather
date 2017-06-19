#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap nws_image(":/splash/weather-QHWeather.png");
    QSplashScreen splash_screen(nws_image);
    splash_screen.show();
    MainWindow w;
    w.show();
    splash_screen.finish(&w);
    return a.exec();
}
