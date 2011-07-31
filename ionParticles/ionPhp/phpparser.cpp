#include "phpparser.h"

extern D_ParserTables parser_tables_ionPhp;

namespace IonPhp {


phpParser::phpParser()
{
    parser.reset(new_D_Parser(&parser_tables_ionPhp, 0));
}

bool phpParser::parse(QString doc)
{
    return dparse(parser.data(), (char *)doc.toStdString().c_str(), doc.length()) && !parser->syntax_errors;
}

}
