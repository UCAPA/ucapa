#include <QApplication>
#include <QSplashScreen>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Navigator");
    a.setApplicationVersion("1.0");

    QPixmap pixmap("ressources/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();

    MainWindow w;
    w.showMaximized();

    splash.finish(&w);
    
    return a.exec();
}
