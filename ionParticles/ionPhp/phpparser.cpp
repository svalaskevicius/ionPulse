#include "phpparser.h"

#include <iostream>
#include <QVector>


int ion_php_parse(IonPhp::phpParser* context);


namespace IonPhp {


phpParser::phpParser()
{
    init_scanner();
}

phpParser::~phpParser()
{
    destroy_scanner();
}

/*void print_tree (pANTLR3_BASE_TREE node, int level = 0) {
    if (!node) {
        return;
    }
    if (node->toString) {
        std::cout << std::string((char *)node->toString(node)->chars);
    } else {
        std::cout << " NIL ";
    }
    std::cout << "; ";
    if (int count = node->getChildCount(node)) {
        std::cout << " ( ";
        for (int i=0; i < count; i++) {
            print_tree((pANTLR3_BASE_TREE)node->getChild(node, i));
        }
        std::cout << " ) ";
    }
}
*/

bool phpParser::parse(QString doc, QString name)
{
    void * buf = setBuf(doc.toAscii().constData());
    int ret = ion_php_parse(this);
    delBuf(buf);

    std::cout << ret << std::endl;

    /*yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE buf = yy_scan_string(doc.toAscii().constData(), scanner);
    //yylex(scanner);
    int tok;
    while ((tok=yylex()) > 0)
              printf("tok=%d  yytext=%s\n", tok, yyget_text(scanner))
    ;
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);*/
    return false;
}


void phpParser::__error(phpParser *myself, const char *error) {
    Q_ASSERT(this == myself);
    std::cerr << "error: " << error << "\n";
}

}
