#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

namespace IonHeart {


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QDir _getPluginsDir();
    void _loadPlugins();
    void _initializePlugin(QObject *plugin);
};

}

#endif // MAINWINDOW_H
