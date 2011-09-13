/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace IonHeart {

namespace Private {


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
};
}
}

#endif // MAINWINDOW_H
