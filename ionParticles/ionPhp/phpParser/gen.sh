
bison -d -o php_parser.cpp php.y
re2c -i --case-inverted -cbdFt php_scanner_defs.h -ophp_scanner.cpp php.l

