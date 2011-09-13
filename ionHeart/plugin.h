/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef INTERFACES_H
#define INTERFACES_H

#include "mainwindow.h"
#include "layout.h"
#include <QList>

namespace IonHeart {

class BasicPlugin {
protected:
    LayoutManager *layoutManager;
public:
    virtual ~BasicPlugin(){}
    void setLayoutManager(LayoutManager *layoutManager) {this->layoutManager = layoutManager;}
    virtual void initialize() = 0;
    virtual QString getName() = 0;
    virtual QList<QString> getDependencies() {
        return QList<QString>();
    }
    virtual void addParent(BasicPlugin * /* parent */) {}
};

}

Q_DECLARE_INTERFACE(IonHeart::BasicPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
