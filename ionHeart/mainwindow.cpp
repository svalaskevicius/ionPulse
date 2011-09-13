/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

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
