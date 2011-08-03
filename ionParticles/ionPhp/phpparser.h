#ifndef PHPPARSER_H
#define PHPPARSER_H

#include <QString>
#include <QScopedPointer>

namespace IonPhp {

class phpParser
{
public:
    phpParser();
    bool parse(QString doc, QString name);
};

}

#endif // PHPPARSER_H
