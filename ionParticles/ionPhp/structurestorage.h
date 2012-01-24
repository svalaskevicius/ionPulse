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

namespace IonPhp {

class ASTNode;
class ASTRoot;

class StructureStorage
{
public:
    StructureStorage(QString connName);
    const QList<QString> &getErrors() const {return errors;}
    QSharedPointer<QSqlQuery> getClasses();
    QSharedPointer<QSqlQuery> getFileClasses(int fileId);
    QSharedPointer<QSqlQuery> getClassMethods(int classId);

    int addFile(QString path);
protected:
    QSqlDatabase db;
    QList<QString> errors;

    void createTables();

    void addClasses(QSqlQuery &classInsertQuery, QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &classes);
    void addMethods(QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &methods);
};


}

#endif // STRUCTURESTORAGE_H
