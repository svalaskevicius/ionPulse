/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "structurestorage.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QtDebug>
#include <QVariant>
#include <QSqlError>

#include "phpparser.h"

namespace IonPhp {

StructureStorage::StructureStorage(QString connName)
    : db(QSqlDatabase::database(connName))
{
    createTables();
}

void StructureStorage::importFileTree(const IonProject::TreeModel &fileSource)
{
    foreach (QString path, getPhpFileList(fileSource)) {
        try {
            addFile(path);
        } catch (std::exception &err) {
            errors << err.what();
            DEBUG_MSG(err.what());
        }
    }

}

QSharedPointer<QSqlQuery> StructureStorage::getClasses()
{
    QSharedPointer<QSqlQuery> query(new QSqlQuery(db));
    query->prepare("select classes.id, classname, filename, line_nr from classes left join files on files.id=classes.file_id");
    if (!query->exec()) {
        qDebug() << query->lastError();
    }
    return query;
}

QSharedPointer<QSqlQuery> StructureStorage::getMethods(int classId)
{
    QSharedPointer<QSqlQuery> query(new QSqlQuery(db));
    query->prepare("select methods.id, methodname, filename, line_nr from methods left join files on files.id=file_id where class_id=:class_id");
    query->bindValue("class_id", classId);
    if (!query->exec()) {
        qDebug() << query->lastError();
    }
    return query;
}

void StructureStorage::createTables()
{
    QSqlQuery query(db);
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        qDebug() << query.lastError();
    }
    if (!query.exec(
        "create table files ("
            "id INTEGER primary key autoincrement, "
            "filename varchar(2048)"
        ")"
    )) {
        qDebug() << query.lastError();
    }
    if (!query.exec(
        "create table classes ("
            "id INTEGER primary key autoincrement, "
            "file_id int,"
            "line_nr int,"
            "classname varchar(255),"
            "FOREIGN KEY(file_id) REFERENCES files(id)"
        ")"
    )) {
        qDebug() << query.lastError();
    }
    if (!query.exec(
        "create table methods ("
            "id INTEGER primary key autoincrement, "
            "file_id int,"
            "line_nr int,"
            "class_id int,"
            "methodname varchar(255),"
            "FOREIGN KEY(file_id) REFERENCES files(id),"
            "FOREIGN KEY(class_id) REFERENCES classes(id)"
        ")"
    )) {
        qDebug() << query.lastError();
    }


}


QVector<QString> StructureStorage::getPhpFileList(const IonProject::TreeModel &fileSource)
{
    QVector<IonProject::TreeItem*> parents;
    QVector<QString> phpFiles;
    parents.push_back(fileSource.getRoot());
    while (!parents.empty()) {
        IonProject::TreeItem *parent = parents.back();
        parents.pop_back();
        foreach (IonProject::TreeItem *child, parent->getChildren()) {
            parents.push_back(child);
            QString path = child->getPath();
            if (path.toLower().endsWith(".php")) {
                phpFiles.push_back(path);
            }
        }
    }
    return phpFiles;
}


void StructureStorage::addFile(QString path)
{
    ASTRoot astRoot = phpParser().parseFile(path);
    //qDebug() << astRoot.dumpXml();

    QSqlQuery fileInsertQuery(db);
    fileInsertQuery.prepare("insert into files(filename) values (:filename)");
    QSqlQuery classInsertQuery(db);
    classInsertQuery.prepare("insert into classes(file_id, line_nr, classname) values(:file_id, :line_nr, :classname)");
    QSqlQuery methodInsertQuery(db);
    methodInsertQuery.prepare("insert into methods(file_id, class_id, line_nr, methodname) values(:file_id, :class_id, :line_nr, :methodname)");

    fileInsertQuery.bindValue("filename", path);
    fileInsertQuery.exec();

    int fileId = fileInsertQuery.lastInsertId().toInt();
    classInsertQuery.bindValue("file_id", fileId);
    methodInsertQuery.bindValue("file_id", fileId);

    addClasses(classInsertQuery, methodInsertQuery, astRoot, astRoot.xpath("//class_declaration"));
}

void StructureStorage::addClasses(QSqlQuery &classInsertQuery, QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &classes)
{
    foreach (pASTNode classDef, classes) {
        pASTNode classLabel = astRoot.xpath("string", classDef).front();

        classInsertQuery.bindValue("line_nr", classLabel->getLine());
        classInsertQuery.bindValue("classname", classLabel->getText());
        if (!classInsertQuery.exec()) {
            qDebug() << classInsertQuery.lastError();
            throw std::runtime_error("failed to register new class definition");
        }
        int classId = classInsertQuery.lastInsertId().toInt();

        methodInsertQuery.bindValue("class_id", classId);
        addMethods(methodInsertQuery, astRoot, astRoot.xpath("class_statement_list/METHOD", classDef));
    }
}


void StructureStorage::addMethods(QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &methods)
{
    foreach(pASTNode methodDef, methods) {
        pASTNode methodLabel = astRoot.xpath("string", methodDef).front();
        methodInsertQuery.bindValue("line_nr", methodLabel->getLine());
        methodInsertQuery.bindValue("methodname", methodLabel->getText());
        if (!methodInsertQuery.exec()) {
            qDebug() << methodInsertQuery.lastError() << methodInsertQuery.lastQuery();
            throw std::runtime_error("failed to register new method definition");
        }
    }
}

}
