#include "syshead.h"

#include "editor.h"
#include "buffer.h"
#include "consts.h"
#include "terminal.h"

/*** editor operations ***/

void editorInsertChar(eState *state, int c) {
    if (state->cy == state->numrows) { // cursor is on eof -> insert empty row first
        editorInsertRow(state, state->numrows, "", 0); 
    }
    erow *row = &state->row[state->cy];
    editorRowInsertChar(state, row, state->cx, c);
    editorMoveCursor(state, ARROW_RIGHT); // advance cursor to after new char
}

void editorInsertNewLine(eState *state) {
    int at = (state->cx == 0) ? state->cy : state->cy + 1;
    int i = 0;
    char *s;
    if (at > 0) { // add indent matching to previous line
        erow *row = &state->row[at - 1];
        s = malloc(row->size + 1);
        while (i < state->cx && 
                (row->chars[i] == '\t' || row->chars[i] == ' ')) {
            s[i] = row->chars[i];
            i++;
        }
    } else {
        s = malloc(1); // We use malloc so we can realloc this safely later if needed
    }
    s[i] = '\0';
    
    if (state->cx == 0) {
        editorInsertRow(state, state->cy, s, i); // insert empty row (possibly with indent)
    } else {
        erow *row = &state->row[state->cy];
        ssize_t size = row->size - state->cx + sizeof(s) + 1; // size of leftovers + indent + nullbyte
        s = realloc(s, size);
        memcpy(&s[i], &row->chars[state->cx], row->size - state->cx);
        editorInsertRow(state, state->cy + 1, s, row->size - state->cx + i);
        row = &state->row[state->cy];
        row->size = state->cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(state, row);
    }
    free(s);
    // Move cursor accross the added indentation + small trick to make sure cursor lands 
    // correctly when inserting on edge of wrap
    if (state->cy == 0 && state->cy == state->cx) {
        editorStepCursor(state, ARROW_RIGHT, i + 1);
    } else {
        editorMoveCursor(state, ARROW_LEFT);
        editorStepCursor(state, ARROW_RIGHT, i + 2);
    }
}

void editorDelChar(eState *state) {
    // Illegal delete locations
    if (state->cy == state->numrows)
        return;
    if (state->cx == 0 && state->cy == 0)
        return;

    erow *row = &state->row[state->cy];
    if (state->cx > 0) {
#ifdef DO_SOFTWRAP
        int prev_wraps = row->wraps;
#endif /* DO_SOFTWRAP */
        editorRowDelChar(state, row, state->cx - 1);
        editorMoveCursor(state, ARROW_LEFT); // adjust the cursor
#ifdef DO_SOFTWRAP
        if (row->wraps < prev_wraps) { // editorUpdateRow was called on DelChar so wraps will be updated
            state->wrapoff--;
            state->iy = recalcIy(state);
            state->ix = recalcIx(state);
        }
#endif /* DO_SOFTWRAP */
        if (state->cx + state->ix == 0 && !(state->cy == 0 && state->cx == state->cy)) {
            // Move cursor left and right to make sure it is rendered on
            // end of the row and not start of next row, in case of wraps.
            editorMoveCursor(state, ARROW_LEFT);
            editorMoveCursor(state, ARROW_RIGHT);
        }
    } else { // deleting on start of row, merge with previous row
        erow *prev_row = &state->row[state->cy - 1];
        int del_row = state->cy;
        editorMoveCursor(state, ARROW_LEFT);
        editorRowAppendString(state, prev_row, row->chars, row->size);
        editorDelRow(state, del_row);
    }
}