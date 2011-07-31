#ifndef PHPPARSER_H
#define PHPPARSER_H

#include <QString>
#include <QScopedPointer>
#include <dparse.h>

namespace IonPhp {

class phpParser
{
public:
    phpParser();
    bool parse(QString doc);
protected:
    QScopedPointer<D_Parser> parser;
};

}

#endif // PHPPARSER_H
