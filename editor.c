#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "buffer.h"
#include "consts.h"
#include "terminal.h"

/*** editor operations ***/

void editorInsertChar(int c) {
    if (E.cy == E.numrows) {
        editorInsertRow(E.numrows, "", 0);
    }
    erow *row = &E.row[E.cy];
    editorRowInsertChar(row, E.cx, c);
    editorMoveCursor(ARROW_RIGHT);
}

void editorInsertNewLine() {
    int at = (E.cx == 0) ? E.cy : E.cy + 1;
    int i = 0;
    char *s;
    if (at > 0) { // add indent matching to previous line
        erow *row = &E.row[at - 1];
        s = malloc(row->size + 1);
        while (i < E.cx && 
                (row->chars[i] == '\t' || row->chars[i] == ' ')) {
            s[i] = row->chars[i];
            i++;
        }
    } else {
        s = malloc(1); // We use malloc so we can realloc this safely later if needed
    }
    s[i] = '\0';
    
    if (E.cx == 0) {
        editorInsertRow(E.cy, s, i);
    } else {
        erow *row = &E.row[E.cy];
        ssize_t size = row->size - E.cx + sizeof(s) + 1; // size of leftovers + indent + nullbyte
        s = realloc(s, size);
        memcpy(&s[i], &row->chars[E.cx], row->size - E.cx);
        editorInsertRow(E.cy + 1, s, row->size - E.cx + i);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(row);
    }
    free(s);
    // Small trick to make sure cursor lands correctly when inserting on edge of wrap
    if (E.cy == 0 && E.cy == E.cx) {
        editorStepCursor(ARROW_RIGHT, i + 1);
    } else {
        editorMoveCursor(ARROW_LEFT);
        editorStepCursor(ARROW_RIGHT, i + 2);
    }
}

void editorDelChar() {
    if (E.cy == E.numrows)
        return;
    if (E.cx == 0 && E.cy == 0)
        return;

    erow *row = &E.row[E.cy];
    if (E.cx > 0) {
#ifdef DO_SOFTWRAP
        int prev_wraps = row->wraps;
#endif /* DO_SOFTWRAP */
        editorRowDelChar(row, E.cx - 1);
        editorMoveCursor(ARROW_LEFT);
#ifdef DO_SOFTWRAP
        if (row->wraps < prev_wraps) {
            E.wrapoff--;
            E.iy = recalcIy();
            E.ix = recalcIx();
        }
#endif /* DO_SOFTWRAP */
        if (E.cx + E.ix == 0 && !(E.cy == 0 && E.cx == E.cy)) {
            /* Move cursor left and right to make sure it is rendered on
             * end of the row and not start of next row, in case of wraps */
            editorMoveCursor(ARROW_LEFT);
            editorMoveCursor(ARROW_RIGHT);
        }
    } else {
        erow *prev_row = &E.row[E.cy - 1];
        int del_row = E.cy;
        editorMoveCursor(ARROW_LEFT);
        editorRowAppendString(prev_row, row->chars, row->size);
        editorDelRow(del_row);
    }
}