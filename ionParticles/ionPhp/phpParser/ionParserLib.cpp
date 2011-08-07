#include "ionParserLib.h"
#include <iostream>

void ion_php_error(const char *error) {
    std::cerr << "error: " << error << "\n";
}
