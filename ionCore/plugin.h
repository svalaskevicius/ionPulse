/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QList>
#include <QMainWindow>


namespace IonCore {

class BasicPlugin {
protected:
    QMainWindow *mainWindow;
public:
    virtual ~BasicPlugin(){}
    void setMainWindow(QMainWindow *mainWindow) {this->mainWindow = mainWindow;}
    virtual void preLoad() {}
    virtual void postLoad() {}
    virtual QString getName() = 0;
    virtual QList<QString> getDependencies() {
        return QList<QString>();
    }
    virtual void addParent(BasicPlugin * /* parent */) {}
};

}

Q_DECLARE_INTERFACE(IonCore::BasicPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
