/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONSHARED_H
#define IONSHARED_H

#include <iostream>


#define CHECK_AND_ADD_PARENT(parent, className, codeToAdd) \
    {\
        if (className::name() == parent->getName() ) {\
            className *target = static_cast<className *>(parent);\
            Q_ASSERT(target);\
            codeToAdd;\
        }\
    }


#include <QDebug>

#define DEBUG_MSG(str) { qDebug() << str; }

#endif // IONSHARED_H
