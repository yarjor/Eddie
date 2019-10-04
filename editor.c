#include "consts.h"
#include "buffer.h"

/*** editor operations ***/

void editorInsertChar(int c) {
    if (E.cy == E.numrows) {
        editorInsertRow(E.numrows, "", 0);
    }
    erow *row = &E.row[E.cy];
    editorRowInsertChar(row, E.cx, c);
    E.cx++;
}

void editorInsertNewLine() {
    if (E.cx == 0) {
        editorInsertRow(E.cy, "", 0);
    } else {
        erow *row = &E.row[E.cy];
        editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(row);
    }
    E.cy++;
    E.cx = 0;
}

void editorDelChar() {
    if (E.cy == E.numrows) return;
    if (E.cx == 0 && E.cy == 0) return;
    
    erow *row = &E.row[E.cy];
    if (E.cx > 0) {
        editorRowDelChar(row, E.cx - 1);
        E.cx--;
    } else {
        erow *prev_row = &E.row[E.cy - 1];
        E.cx = prev_row->size;
        editorRowAppendString(prev_row, row->chars, row->size);
        editorDelRow(E.cy);
        E.cy--;
    }
}