/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPAPI_H
#define PHPAPI_H

/**
 * \brief Provides editor support for php files.
 *
 * PHP plugin extends IonEditor and IonProject plugins,
 * adding php related functionality to both of them.
 *
 * IonEditor is extended to provide syntax highlighting for php
 * files. Also, in-editor code navigation is planned.
 *
 * IonProject plugins is extended to decorate the project tree view and
 * add php file structure inside TreeItem objects for php files.
 *
 * Currently php file type famility is described by the plugin as "text/php"
 * and includes files with extentions *.php, *.php3, *.phtml
 */
namespace IonPhp {
}

#endif // PHPAPI_H
