/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DIRECTORYTREESOURCE_H
#define DIRECTORYTREESOURCE_H

#include "projectapi.h"
#include <QDir>

namespace IonProject {
namespace Private {

class DirectoryTreeSource : public TreeModelSource {
public:
    class DirectoryInfo {
    private:
        QDir dir;
    public:
        DirectoryInfo(QString path) : dir(path) {}
        virtual ~DirectoryInfo() {}
        inline virtual QStringList dirnames() {return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);}
        inline virtual QStringList filenames() {return dir.entryList(QDir::Files, QDir::Name);}
        inline virtual QString absolutePath() {return dir.absolutePath();}
    };

    DirectoryTreeSource(const QString &initialDir)
        : root(NULL), initialDir(initialDir) {
    }
    DirectoryTreeSource()
        : initialDir("") {
    }
    virtual TreeItem * setupData(QString pathFilter = "");
    virtual QString getTitle() const {return "Project Browser";}
protected:
    void addDirectory(TreeItem *parent, QString pathFilter);
    void _updateDirectory(TreeItem* currentTreeItemsParent, QList<TreeItem*> &parents);
    TreeItem *findChildForPath(TreeItem *node, QString path);
    inline virtual QSharedPointer<DirectoryInfo> _getDir(const QString path);
private:
    TreeItemImpl* root;
    QString initialDir;
};

}
}

#endif
