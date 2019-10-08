#ifndef FILETYPE_H
#define FILETYPE_H

#include "consts.h"
#include "highlight.h"

/*** C consts ***/
char *C_HL_EXTENSIONS[] = { ".c", ".h", ".cpp", NULL };
char *C_HL_KEYWORDS[] = {
    "switch", "if", "while", "for", "break", "continue", "return", "else",
    "struct", "union", "typedef", "static", "enum", "class", "case",

    "int|", "long|", "double|", "float|", "char|", "unsinged|", "signed|",
    "void|",
    NULL
};

/**
 * @brief Database of syntax-highlighting rules
 * 
 */
struct editorSyntax HLDB[] = {
    {
        "c",
        C_HL_EXTENSIONS,
        C_HL_KEYWORDS,
        "//",
        "/*", "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_HASHTAG | HL_HIGHLIGHT_LTGT
        },
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

#endif