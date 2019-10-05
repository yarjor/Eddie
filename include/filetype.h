#ifndef FILETYPE_H
#define FILETYPE_H

#include "consts.h"
#include "highlight.h"

char *C_HL_EXTENSIONS[] = { ".c", ".h", ".cpp", NULL };

/**
 * @brief Database of syntax-highlighting rules
 * 
 */
struct editorSyntax HLDB[] = {
    {
        "c",
        C_HL_EXTENSIONS,
        HL_HIGHLIGHT_NUMBERS
    },
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

#endif