/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#pragma once

#include <QObject>
#include <ionHeart/plugin.h>

#define LAYOUT_PLUGIN_NAME "ionLayout"

namespace IonLayout {


class Plugin : public QObject, public IonHeart::BasicPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonHeart::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    void initialize();
    QString getName() {
        return LAYOUT_PLUGIN_NAME;
    }
signals:

public slots:
protected slots:

};

}





