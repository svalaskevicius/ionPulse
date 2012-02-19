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
#include <boost/shared_ptr.hpp>

namespace IonProject {
namespace Private {

class DirectoryTreeSource : public TreeModelSource {
public:
    class DirectoryInfo {
    private:
        QDir dir;
    public:
        DirectoryInfo(QString path) : dir(path) {}
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
    virtual TreeItem * setupData();
    virtual QString getTitle() const {return "Project Browser";}
protected:
    void addDirectory(TreeItem *parent);
    TreeItem *findChildForPath(TreeItem *node, QString path);
    inline virtual boost::shared_ptr<DirectoryInfo> _getDir(const QString path);
private:
    TreeItemImpl* root;
    QString initialDir;
};

}
}

#endif
