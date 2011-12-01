/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef STRUCTURESTORAGE_H
#define STRUCTURESTORAGE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <ionParticles/ionProject/projectapi.h>

namespace IonPhp {

class StructureStorage
{
public:
    StructureStorage(QString connName);
    void importFileTree(const IonProject::TreeModel &fileSource);
    const QList<QString> &getErrors() const {return errors;}
    QSharedPointer<QSqlQuery> getClasses();
    QSharedPointer<QSqlQuery> getMethods(int classId);
protected:
    QSqlDatabase db;
    QList<QString> errors;

    void createTables();
    QVector<QString> getPhpFileList(const IonProject::TreeModel &fileSource);
    void addFile(QString path);
};


}

#endif // STRUCTURESTORAGE_H
