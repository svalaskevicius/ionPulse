/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DIRECTORYTREESOURCE_H
#define DIRECTORYTREESOURCE_H

#include "projectapi.h"

namespace IonProject {
namespace Private {

class DirectoryTreeSource : public TreeModelSource {
public:
    DirectoryTreeSource(const QString &initialDir)
        : root(NULL), initialDir(initialDir) {
    }
    DirectoryTreeSource()
        : initialDir("") {
    }
    virtual TreeItem * setupData();
    virtual QString getTitle() const {return "Project Browser";}
protected:
    void addDirectory(TreeItem *parent, QString directory);
private:
    TreeItemImpl* root;
    QString initialDir;
};

}
}

#endif
