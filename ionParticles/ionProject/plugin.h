/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONEDITORPROJECT_H
#define IONEDITORPROJECT_H

#include <QObject>
#include <ionCore/plugin.h>
#include <ionParticles/ionEditor/editorapi.h>
#include <ionParticles/ionLayout/layoutapi.h>
#include "projectapi.h"
#include "treemodeladapter.h"
#include "treeview.h"
#include "treeitem.h"

namespace IonProject {

namespace Private {

class Plugin : public QObject, public ProjectPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ionPulse.ionProject")
    Q_INTERFACES(IonCore::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    ~Plugin() {}
    void postLoad();
    QString getName() {
        return ProjectPlugin::name();
    }
    virtual QList<QString> getDependencies() {
        QList<QString> ret;
        ret.append(IonLayout::LayoutPlugin::name());
        ret.append(IonEditor::EditorPlugin::name());
        return ret;
    }
    virtual void addParent(BasicPlugin *parent);
    QSharedPointer<TreeModelAdapter> getProjectFileTreeModel();
    virtual void addTreeWidget(QSharedPointer<TreeModelAdapter> model);
    virtual void addTreeWidget(QSharedPointer<TreeModelSource> modelSource);
    virtual QSharedPointer<TreeItemFactory> createTreeItemFactory();
    virtual const boost::function<QSharedPointer<TreeModelSource> (QString dirname)> &getTreeModelSourceFactory();
    virtual void setTreeModelSourceFactory(boost::function<QSharedPointer<TreeModelSource> (QString dirname)> factory);
    virtual void registerJsApi(QJSEngine &jsEngine);

private:
    struct _defaultTreeModelSourceFactory {
        QSharedPointer<TreeModelSource> operator()(QString dirname);
    };

    IonEditor::EditorPlugin *editorPlugin;
    IonLayout::LayoutManager *layoutManager;
    QSharedPointer<TreeModelAdapter> projectTreeModel;

    boost::function<QSharedPointer<TreeModelSource> (QString dirname)> _treeModelSourceFactory;

signals:

public slots:
    void onNewProject();
    void onUpdateProject();
protected slots:
    void openFile(QString path, int);

};

}
}

Q_DECLARE_METATYPE(IonProject::TreeItem*)
Q_DECLARE_METATYPE(IonProject::Private::TreeView*)

#endif // IONEDITORPROJECT_H




