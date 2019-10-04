#ifndef BUFFER_H
#define BUFFER_H

#include "consts.h"

/*** row operations ***/

int editorRowCxToRx(erow *row, int cx);

void editorUpdateRow(erow *row);

void editorAppendRow(char *s, size_t len);

void editorRowInsertChar(erow *row, int at, int c);

/*** append buffer ***/

struct abuf
{
    char *b;
    int len;
};

#define ABUF_INIT \
{   \
    NULL, 0 \
}

void abAppend(struct abuf *ab, const char *s, int len);

void abFree(struct abuf *ab);

#endif