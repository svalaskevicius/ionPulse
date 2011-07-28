#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QDir>


class IonPlugin;

namespace IonHeart {


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QMap<QString, IonPlugin *> _includedPlugins;
    QDir _getPluginsDir();
    void _loadPlugins();
    bool _arePluginsIncluded(QStringList pluginNames);
    void _initializePlugin(IonPlugin *plugin);
};

}

#endif // MAINWINDOW_H
