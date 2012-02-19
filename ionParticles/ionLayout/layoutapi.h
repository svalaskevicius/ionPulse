/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>
#include <ionCore/plugin.h>

/**
 * \brief Provides facilities for simple and flexible widget layout management.
 *
 * The purpose of this plugin is to provide an easy solution for the layout handling.
 * This is achieved by using flexible zones as targets for the panels. Each zone is
 * a named entity, positioned in the main zones tree. Panels can then be positioned by
 * providing a UNIX like path to the target zone.
 */
namespace IonLayout {

/**
 * \brief Zone properties.
 *
 * The ZoneDefinition entity is used to define a zone - assign zone properties, which
 * then could be passed to create a zone using LayoutManager::addZone().
 */
struct ZoneDefinition
{
    /**
     * \brief UNIX like path to where this zone should be positioned.
     */
    QString parentPath;

    /**
     * \brief Zone name.
     */
    QString name;

    /**
     * \brief Orientation of child zones.
     */
    Qt::Orientation orientation;

    /**
     * \brief Can be used to position a zone amongst sibling-level zones in the tree.
     */
    QString after, before;

    /**
     * \brief Should the zone be hidden if there are no panels subscribed?
     */
    bool hideIfEmpty;

    /**
     * \brief Relative weight [1..1000] of the zone width/height comparing to the sibling-level zones.
     */
    int sizeWeight;

    /**
     * \brief Can subscribed panels be closed?
     */
    bool childrenClosable;
};

/**
 * \brief Interface of the zone client panel.
 *
 * For a widget to be added to the zone, a PanelWidget should be provided to LayoutManager::add().
 * It provides all the info needed for the widget to be positioned in the zones tree.
 */
class PanelWidget
{
public:
    /**
     * \brief Retrieve QWidget to be added.
     */
    virtual QWidget *getWidget() = 0;

    /**
     * \brief A title to be displayed.
     */
    virtual QString getPanelTitle() = 0;

    /**
     * \brief A UNIX-like path to the target zone.
     */
    virtual QString getPanelZone() = 0;
};

/**
 * \brief Provides a frontend to the layout management tasks.
 *
 * The interface defines main zone and panel management tasks such as addition and removal.
 */
class LayoutManager : public QObject
{
    Q_OBJECT
public:
    virtual ~LayoutManager() {}

    /**
     * \brief Add a PanelWidget to the zones tree.
     */
    virtual void add(PanelWidget *panel) = 0;

    /**
     * \brief Remove a PanelWidget from the zones tree.
     */
    virtual void remove(PanelWidget *panel) = 0;

    /**
     * \brief Focus a PanelWidget.
     */
    virtual void focus(PanelWidget *panel) = 0;

    /**
     * \brief Add new zone to the tree.
     */
    virtual void addZone(ZoneDefinition &zone) = 0;
public slots:

    /**
     * \brief Refresh the panel title label.
     */
    virtual void updatePanelTitle(PanelWidget *panel) = 0;
};


/**
 * \brief The interface of the IonLayout plugin.
 *
 * The LayoutPlugin interface provides an unified application scope
 * access to the LayoutManager.
 *
 * To use it, client plugins should list IonLayout::LayoutPlugin::name()
 * in their dependencies. This way the client plugins will receive an
 * instance of LayoutPlugin in their IonCore::BasicPlugin::addParent() method.
 */
class LayoutPlugin : public IonCore::BasicPlugin
{
public:
    /**
     * \brief Retrieve LayoutManager instance.
     */
    virtual LayoutManager *getLayoutManager() = 0;

    /**
     * \brief Plugin name to be used in IonCore::BasicPlugin::getDependencies()
     *        method of the subscribed plugins.
     */
    static QString name() { return "ionLayout"; }
};

}

#endif // PANELWIDGET_H
