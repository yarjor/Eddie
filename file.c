#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "terminal.h"
#include "buffer.h"

void editorOpen(char *filename) {
    free(E.filename);
    E.filename = strdup(filename);

    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorAppendRow(line, linelen);
    }
    free(line);
    fclose(fp);
}