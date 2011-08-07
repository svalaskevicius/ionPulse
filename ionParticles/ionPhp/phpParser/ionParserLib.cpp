#include "ionParserLib.h"
#include <iostream>

_php_scanner_globals php_scanner_globals;

void ion_php_error(const char *error) {
    std::cerr << "error: " << error << "\n";
}
