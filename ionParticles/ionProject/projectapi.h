#pragma once

#include <QAbstractItemModel>
#include <ionCore/plugin.h>

#include <boost/function.hpp>

#define TREESOURCE_CLASS_DIR  "dir"
#define TREESOURCE_CLASS_FILE "file"

/**
 * \brief Provides basic project files management and facilities to extend project listing.
 *
 * The purpose of this plugin is to provide an easy solution for the project files handling.
 * This is achieved by using Qt model/view architecture and providing means to decorate the
 * list of files shown using IonProject::TreeModelSource interface.
 */
namespace IonProject {

/**
 * \brief Describes a file tree element model and provides file tree management functionality.
 *
 * The TreeItem class describes the atomic file tree element, which may be either at leaf
 * level or contain children (also TreeItem objects).
 */
class TreeItem : public QObject
{
    Q_OBJECT
public:
    virtual ~TreeItem() {}

    /**
     * \brief Retrieve data to be displayed.
     */
    Q_INVOKABLE virtual QVariant data(int column) const = 0;

    /**
     * \brief The row number counting from the parent node.
     *
     * Retrieve the position at which this element should be displayed.
     * It is the position of this item amongst its siblings at the tree structure.
     */
    Q_INVOKABLE virtual int getRowNr() = 0;

    /**
     * \brief Get the parent item.
     */
    Q_INVOKABLE virtual TreeItem *parent() = 0;

    /**
     * \brief Retrieve the count of children items,
     */
    Q_INVOKABLE virtual int visibleChildrenCount() const = 0;

    /**
     * \brief Retrieve a list of children assigned to this TreeItem.
     */
    Q_INVOKABLE virtual QList<TreeItem*> &getChildren() = 0;

    /**
     * \brief Clear all the children freeing any memory they allocated.
     */
    Q_INVOKABLE virtual void clearChildren() = 0;

    /**
     * \brief Set this, and children TreeItem objects to be filtered by the given string.
     *
     * The filter modifies visibility flag, and recurses to children TreeItems. If
     * this TreeItem does not satisfy the filter query, but one or more children do, this
     * item would also be set as visible.
     *
     * \see isVisible()
     */
    Q_INVOKABLE virtual void filter(QString const filter) = 0;

    /**
     * \brief Set this, and children TreeItem objects to be filtered by the given regular expression.
     *
     * The filter modifies visibility flag, and recurses to children TreeItems. If
     * this TreeItem does not satisfy the filter query, but one or more children do, this
     * item would also be set as visible.
     *
     * \see isVisible()
     */
    Q_INVOKABLE virtual void filterByRegexp(QRegExp const filter) = 0;

    /**
     * \brief Retrieve TreeItem visibility state.
     *
     * The visibility state defines if the item should be shown in the project
     * tree. This flag can be affected by either setFullVisibility() or filter()
     * methods.
     *
     * \see setFullVisibility()
     * \see filter()
     */
    Q_INVOKABLE virtual bool isVisible() const = 0;

    /**
     * \brief Set given visibility flag to this TreeItem and to all of the children (recursively).
     */
    Q_INVOKABLE virtual void setFullVisibility(bool visible) = 0;

    /**
     * \brief Get file path to be opened in the Editor on activating this TreeItem.
     *
     * If file path would be missing or file would be not existing, the Editor would
     * not be opened.
     */
    Q_INVOKABLE virtual QString getPath() const = 0;

    /**
     * \brief Get the line number to focus in the Editor on activating this TreeItem.
     *
     * If the line returned is -1, the Editor focus for the file will not change.
     */
    Q_INVOKABLE virtual int getLine() const = 0;

    /**
     * \brief Add a child item to the TreeItem structure.
     */
    Q_INVOKABLE virtual void appendChild(TreeItem *child) = 0;

    /**
     * \brief Retrieve the child from the specified position.
     */
    Q_INVOKABLE virtual TreeItem *getVisibleChildByNr(int rowNr) = 0;

    /**
     * \brief Retrieve child position amongst its siblings given its address location.
     */
    Q_INVOKABLE virtual int getChildRowNr(TreeItem *child) = 0;

    /**
     * \brief Retrieve TreeItem class (CSS notation).
     */
    Q_INVOKABLE virtual const QString &getItemClass() const = 0;
};


/**
 * \brief A factory class to instantiate TreeItem objects.
 */
class TreeItemFactory {
public:
    virtual ~TreeItemFactory() {}

    /**
     * \brief A factory method to instantiate TreeItem objects.
     */
    virtual TreeItem* createTreeItem(QString itemClass, QString const name, QString filterBy, QString const path, int const line, TreeItem *parent) = 0;
};


/**
 * \brief A tree source interface defining API to build a TreeItem structure.
 *
 * Objects of this class are used to generate a tree of TreeItem objects to be displayed in project view widget.
 *
 * \see ProjectPlugin::setTreeModelSourceFactory
 */
class TreeModelSource {
public:
    virtual ~TreeModelSource() {}

    /**
     * \brief The main generator entry point, returning the root element of the created structure.
     *
     * \param pathFilter filter only files under the given path to setup
     */
    virtual TreeItem * setupData(QString pathFilter = "") = 0;

    /**
     * \brief Return a title to be displayed for the generated tree widget.
     */
    virtual QString getTitle() const = 0;
};


/**
 * \brief The interface of the IonProject plugin.
 *
 * The ProjectPlugin interface provides an unified application scope
 * access to modify TreeModelSource factory and set their own factory
 * using a Decorator Pattern to extend the project tree functionality.
 *
 * To use it, client plugins should list IonProject::ProjectPlugin::name()
 * in their dependencies. This way the client plugins will receive an
 * instance of ProjectPlugin in their IonCore::BasicPlugin::addParent() method.
 */
class ProjectPlugin : public IonCore::BasicPlugin {
public:
    /**
     * \brief Add a tree widget based on the provided TreeModelSource generator.
     */
    virtual void addTreeWidget(QSharedPointer<TreeModelSource> modelSource) = 0;

    /**
     * \brief Retrieve a factory instance to create basic TreeItem nodes.
     */
    virtual QSharedPointer<TreeItemFactory> createTreeItemFactory() = 0;

    /**
     * \brief Plugin name to be used in IonCore::BasicPlugin::getDependencies() method of the subscribed plugins.
     */
    static QString name() {return "ionProject";}

    /**
     * \brief Retrieve the set factory to instantiate a TreeModelSource for the project files tree.
     */
    virtual const boost::function<QSharedPointer<TreeModelSource> (QString dirname)> &getTreeModelSourceFactory() = 0;

    /**
     * \brief Set the factory to instantiate a TreeModelSource for the project files tree.
     */
    virtual void setTreeModelSourceFactory(boost::function<QSharedPointer<TreeModelSource> (QString dirname)> factory) = 0;
};

}
