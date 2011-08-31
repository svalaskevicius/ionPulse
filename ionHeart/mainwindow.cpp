#include "mainwindow.h"
#include <shared.h>
#include <QStatusBar>

namespace IonHeart {

namespace Private {



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    resize(1024, 800);
    setWindowTitle(QObject::tr("ionPulse"));
    setStatusBar(new QStatusBar(this));
}

MainWindow::~MainWindow()
{
}
}

}
