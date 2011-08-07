#ifndef IONPARSERLIB_H
#define IONPARSERLIB_H

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QMap>
#include <QVector>

class ASTNode;
typedef ASTNode *pASTNode;

class ASTNode {
protected:
    QString name;
    QList<pASTNode> children;
    QMap<QString, QString> strdata;
public:
    ASTNode(QString name) : name(name) {}
    pASTNode addChild(pASTNode child) {children.append(child);return this;}
    pASTNode addData(QString name, QString data) {strdata[name]=data; return this;}
    static pASTNode create(QString name) {return new ASTNode(name);}
};

int ion_php_lex(pASTNode *astNode);
void ion_php_error(const char *error);

struct _php_scanner_globals {
        //zend_file_handle *yy_in;
        //zend_file_handle *yy_out;

        unsigned int yy_leng;
        unsigned char *yy_start;
        unsigned char *yy_text;
        unsigned char *yy_cursor;
        unsigned char *yy_marker;
        unsigned char *yy_limit;
        int yy_state;
        QVector<int> state_stack;
};

typedef struct _zend_lex_state {
        unsigned int yy_leng;
        unsigned char *yy_start;
        unsigned char *yy_text;
        unsigned char *yy_cursor;
        unsigned char *yy_marker;
        unsigned char *yy_limit;
        int yy_state;
        QVector<int>  state_stack;

        //zend_file_handle *in;
        uint lineno;
        char *filename;
} zend_lex_state;


extern _php_scanner_globals php_scanner_globals;
#endif // IONPARSERLIB_H
