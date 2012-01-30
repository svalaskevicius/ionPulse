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

#include <QSharedPointer>

#include "phpParser/ionParserLib.h"

namespace IonPhp {

class ASTNode;
class ASTRoot;

class StructureStorage
{
public:
    StructureStorage(QString connName);
    QSharedPointer<QSqlQuery> getClasses();
    QSharedPointer<QSqlQuery> getFile(QString filename);
    QSharedPointer<QSqlQuery> getFileClasses(int fileId);
    QSharedPointer<QSqlQuery> getClassMethods(int classId);

    void removeFile(int file_id);

    int addFile(QString path, int timestamp, ASTRoot &astRoot);

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
protected:
    QSqlDatabase db;

    void createTables();

    void addClasses(QSqlQuery &classInsertQuery, QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &classes);
    void addMethods(QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &methods);
};


}

#endif // STRUCTURESTORAGE_H
