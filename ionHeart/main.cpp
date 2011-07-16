#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IonHeartWindow w;
    w.show();

    return a.exec();
}
