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
