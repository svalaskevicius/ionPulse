/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QList>
#include <QMainWindow>

/**
 * \brief Holds the bare window creation and plugin management functionality.
 *
 * The IonCore module is the whole application.
 * The main responsibilities of the module are:
 * - create the QApplication and bare main window;
 * - load and initialise plugins.
 */
namespace IonCore {

/**
 * \brief Defines an interface for every loadable plugin.
 *
 * Every plugin has to implement this interface for it to be processed
 * by the plugin loader. The interface defines several methods to controll
 * how (BasicPlugin::preLoad, BasicPlugin::postLoad) and when
 * (BasicPlugin::getDependencies) each plugin is initialised.
 */
class BasicPlugin
{
protected:
    /**
     * \brief Main application window handle.
     *
     * The contents of this pointer is set automatically by the plugin management system
     * before the plugin functionality is invoked. This variable can be used to decorate
     * the application window or attach menu items.
     *
     * Note: the general approach to add widgets to the application is to use IonLayout
     * plugin, which is suited more (than direct modification) in most of the cases.
     */
    QMainWindow *mainWindow;

public:
    /**
     * \brief Class desctructor.
     */
    virtual ~BasicPlugin() {}

    /**
     * \brief Main window handle setter.
     *
     * Usually this method is invoked automatically before the plugin functionality is
     * executed. It sets the BasicPlugin::mainWindow variable which could later be used to
     * access the main application window.
     *
     * \see BasicPlugin::mainWindow
     */
    void setMainWindow(QMainWindow *mainWindow) {this->mainWindow = mainWindow;}
    virtual void preLoad() {}
    virtual void postLoad() {}
    virtual QString getName() = 0;
    virtual QList<QString> getDependencies() {
        return QList<QString>();
    }
    virtual void addParent(BasicPlugin * /* parent */) {}
};

}

Q_DECLARE_INTERFACE(IonCore::BasicPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
