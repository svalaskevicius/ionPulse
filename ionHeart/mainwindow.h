#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>


class IonHeartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit IonHeartWindow(QWidget *parent = 0);
    ~IonHeartWindow();

private:
    QDir _getPluginsDir();
    void _loadPlugins();
    void _initializePlugin(QObject *plugin);
};

#endif // MAINWINDOW_H
