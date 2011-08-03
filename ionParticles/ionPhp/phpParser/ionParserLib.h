#ifndef IONPARSERLIB_H
#define IONPARSERLIB_H

typedef enum {PHP, TEXT} IonPhpParserState;
typedef struct {
    IonPhpParserState state;
} IonPhpParserOptions;
extern IonPhpParserOptions ionPhpParserOptions;

#endif // IONPARSERLIB_H
