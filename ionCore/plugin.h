/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtCore/QList>
#include <QtWidgets/QMainWindow>
#include <QtScript/QScriptEngine>

/**
 * \brief Holds the bare window creation and plugin management functionality.
 *
 * The IonCore module is the whole application.
 * The main responsibilities of the module are:
 * - create the QApplication and bare main window;
 * - load and initialise plugins.
 *
 * After the above steps all the functionality is deferred to the plugins, keeping
 * the application layer thin.
 */
namespace IonCore {

/**
 * \brief Defines an interface for every loadable plugin.
 *
 * Every plugin has to implement this interface for it to be processed
 * by the plugin loader. The interface defines several methods to control
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

    /**
     * \brief Early plugin initialisation.
     *
     * This method is invoked before the plugin dependencies are loaded and could
     * be used to bootstrap the plugin's environment.
     *
     * \see postLoad
     */
    virtual void preLoad() {}

    /**
     * \brief Plugin initialisation.
     *
     * This method is invoked after the plugin's dependencies are loaded and should
     * normally be used to initialise the plugin.
     */
    virtual void postLoad() {}

    /**
     * \brief Unique plugin name.
     *
     * The name returned by this method uniquely describes the plugin and is used in
     * the plugin dependencies management.
     *
     * \see getDependencies
     */
    virtual QString getName() = 0;

    /**
     * \brief List of the plugin names that should be initialised before this plugin.
     *
     * The plugin BasicPlugin::postLoad method will only be invoked after all the listed
     * plugins are found and initialised.
     *
     * \see getName
     */
    virtual QList<QString> getDependencies() {
        return QList<QString>();
    }

    /**
     * \brief Notifies the loaded parent plugin.
     *
     * During the plugin dependencies loading the load manager notifies the dependant plugins
     * about each loaded parent. This gives the plugin a chance to interact which each
     * dependency using an API defined by the parent plugin.
     *
     * \see CHECK_AND_ADD_PARENT
     */
    virtual void addParent(BasicPlugin * /* parent */) {}

    /**
     * \brief Allows plugin to register its own JS api
     *
     * Each plugin can extend JS API by setting properties on the provided QScriptEngine instance.
     */
    virtual void registerJsApi(QScriptEngine & /* jsEngine */) {}
};

}

Q_DECLARE_INTERFACE(IonCore::BasicPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
