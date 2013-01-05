/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONEDITORPHP_H
#define IONEDITORPHP_H

#include <QObject>
#include <ionCore/plugin.h>

#include <ionParticles/ionEditor/editorapi.h>
#include <ionParticles/ionProject/projectapi.h>

#include "structurestorage.h"

#include "phpparser.h"

#define PHP_PLUGIN_NAME "ionPhp"

namespace IonPhp {
namespace Private {


class Plugin : public QObject, public IonCore::BasicPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ionPulse.ionPhp")
    Q_INTERFACES(IonCore::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    void preLoad();
    void postLoad();
    QString getName() {
        return PHP_PLUGIN_NAME;
    }
    virtual QList<QString> getDependencies() {
        QList<QString> ret;
        ret.append(IonEditor::EditorPlugin::name());
        ret.append(IonProject::ProjectPlugin::name());
        ret.append(IonDbXml::DbXmlPlugin::name());
        return ret;
    }
    virtual void addParent(BasicPlugin *parent);
    virtual void registerJsApi(QScriptEngine & jsEngine);
protected:
    struct phpTreeModelSourceDecoratorFactory {
    private:
        boost::function<QSharedPointer<IonProject::TreeModelSource>(QString dirname)> origFactory;
        StructureStorage &structureStorage;
        QSharedPointer<IonProject::TreeItemFactory> treeItemFactory;
    public:
        phpTreeModelSourceDecoratorFactory(
                    const boost::function<QSharedPointer<IonProject::TreeModelSource>(QString dirname)> &origFactory,
                    StructureStorage &structureStorage,
                    QSharedPointer<IonProject::TreeItemFactory> treeItemFactory
                )
            : origFactory(origFactory), structureStorage(structureStorage), treeItemFactory(treeItemFactory)
        {
        }
        QSharedPointer<IonProject::TreeModelSource> operator()(QString dirname);
    };

    Q_INVOKABLE IonPhp::Private::PhpParser* createParser() {
        return new IonPhp::Private::PhpParser();
    }

private:
    IonEditor::EditorPlugin *editorPlugin;
    IonProject::ProjectPlugin *projectPlugin;
    IonDbXml::DbXmlPlugin *dbXmlPlugin;

    QSharedPointer<StructureStorage> structureStorage;

signals:

public slots:

};

}
}

Q_DECLARE_METATYPE(IonPhp::Private::PhpParser*)

#endif // IONEDITORPHP_H




