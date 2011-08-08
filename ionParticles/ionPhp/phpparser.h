#ifndef PHPPARSER_H
#define PHPPARSER_H

#include <QString>
#include <QScopedPointer>

namespace IonPhp {

class phpParser
{
public:
    phpParser();
    ~phpParser();
    bool parse(QString doc, QString name);
protected:
   void init_scanner();
   void destroy_scanner();
   void *setBuf(const char*);
   void delBuf(void *);
public:
    void* __scanner;
    void __error(phpParser *myself, const char *error);
};

}

#endif // PHPPARSER_H
