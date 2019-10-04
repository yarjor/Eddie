#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

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


char *editorRowsToString(int *buflen) {
    int totlen = 0;
    int j;

    for (j = 0; j < E.numrows; j++)
        totlen += E.row[j].size + 1; // extra char for newline
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *loc = buf;
    for (j = 0; j < E.numrows; j++) {
        memcpy(loc, E.row[j].chars, E.row[j].size);
        loc += E.row[j].size;
        *loc = '\n';
        loc++;
    }

    return buf;
}


void editorSave() {
    if (E.filename == NULL) return;

    int len;
    char *buf = editorRowsToString(&len);

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    ftruncate(fd, len);
    write(fd, buf, len);
    close(fd);
    free(buf);
}