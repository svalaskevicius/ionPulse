/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONSHARED_H
#define IONSHARED_H

#include <iostream>
#include <QDebug>

/**
 * \brief Helper code to apply the correct code to different loaded plugin dependencies.
 *
 * This definition can be used to simplify parent plugin notification handling
 * on the client side.
 *
 * \see IonCore::BasicPlugin::addParent
 */
#define CHECK_AND_ADD_PARENT(parent, className, codeToAdd) \
    {\
        if (className::name() == parent->getName() ) {\
            className *target = static_cast<className *>(parent);\
            Q_ASSERT(target);\
            codeToAdd;\
        }\
    }


/**
 * \brief Debug message output.
 *
 * Handles the debug output in one place for easier code management.
 */
#define DEBUG_MSG(str) { qDebug() << str; }

#endif // IONSHARED_H
