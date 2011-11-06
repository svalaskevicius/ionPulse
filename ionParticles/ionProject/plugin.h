/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONEDITORPROJECT_H
#define IONEDITORPROJECT_H

#include <QObject>
#include <ionHeart/plugin.h>
#include <ionParticles/ionEditor/editorapi.h>
#include <ionParticles/ionLayout/layoutapi.h>
#include "projectapi.h"


namespace IonProject {

class TreeModel;

class Plugin : public QObject, public ProjectPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonHeart::BasicPlugin)
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
    TreeModel *getProjectFileTreeModel();
    virtual void addTreeWidget(TreeModel *model);
    virtual void addTreeWidget(TreeModelSource *modelSource);
    virtual TreeItemFactory* createTreeItemFactory();

private:
    IonEditor::EditorPlugin *editorPlugin;
    IonLayout::LayoutManager *layoutManager;
    QSharedPointer<TreeModel> projectTreeModel;
signals:

public slots:
protected slots:
    void openFile(QString path);

};

}

#endif // IONEDITORPROJECT_H




