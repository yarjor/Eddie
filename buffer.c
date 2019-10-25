#include "syshead.h"

#include "buffer.h"
#include "consts.h"
#include "highlight.h"
#include "structs.h"

int distance_to_next_space(erow *row, int at) {
    int j;
    for (j = at + 1; j < row->size; j++) {
        if (isspace(row->chars[j]))
            break;
    }
    return j - at;
}

int distance_from_prev_space(erow *row, int at) {
    int j;
    for (j = at - 1; j >= 0; j--) {
        if (isspace(row->chars[j]))
            break;
    }
    return at - j;
}

/*** row operations ***/

int editorRowCxToRx(erow *row, int cx) {
    int rx = 0;
    int j;
    for (j = 0; j < cx; j++) {
        if (row->chars[j] == '\t')
            rx += (EDDIE_TAB_STOP - 1) - (rx % EDDIE_TAB_STOP); // replace tab count with amount of spaces to hit next tab stop
        rx++;
    }
    return rx;
}

int editorRowRxToCx(erow *row, int rx) {
    int cur_rx = 0;
    int cx;
    for (cx = 0; cx < row->size; cx++) {
        if (row->chars[cx] == '\t')
            cur_rx += (EDDIE_TAB_STOP - 1) - (cur_rx % EDDIE_TAB_STOP);
        cur_rx++;

        if (cur_rx > rx)
            return cx;
    }
    return cx;
}

void editorUpdateRow(erow *row) {
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t')
            tabs++;

    free(row->render);
    int size = (row->size + tabs * (EDDIE_TAB_STOP - 1)) + 1;
    row->render = malloc(size);

    int idx = 0;
    int row_idx = 0;
    row->wraps = 0;
    row->wrap_stops = malloc(sizeof(int));
    row->wrap_stops[0] = E.editcols;
    for (j = 0; j < row->size; j++) {
#ifdef DO_SOFTWRAP
        int to_next_space = distance_to_next_space(row, j);
        int from_prev_space = distance_from_prev_space(row, j);
        if ((isspace(row->chars[j]) && // next word will overflow the display
                row_idx + to_next_space >= E.editcols) ||
            (row_idx >= E.editcols && // word is too long to avoid breaking
                from_prev_space >= E.editcols / 2)) {
            row->wraps++;
            row->wrap_stops = realloc(row->wrap_stops, row->wraps * sizeof(int));
            row->wrap_stops[row->wraps - 1] = row_idx;
            row_idx = 0;
            size++;
            row->render = realloc(row->render, size);
            row->render[idx++] = '\n';
        }
#endif /* DO_SOFTWRAP */
        if (row->chars[j] == '\t') {
            row->render[idx++] = ' ';
            row_idx++;
            while (idx % EDDIE_TAB_STOP != 0) {
                row->render[idx++] = ' ';
                row_idx++;
            }
        } else {
            row->render[idx++] = row->chars[j];
            row_idx++;
        }
    }
#ifdef DO_SOFTWRAP
    row->wrap_stops = realloc(row->wrap_stops, (row->wraps + 1) * sizeof(int));
    row->wrap_stops[row->wraps] = row_idx;
#endif /* DO_SOFTWRAP */
    row->render[idx] = '\0';
    row->rsize = idx;

    editorUpdateSyntax(row);
}

void editorInsertRow(int at, char *s, size_t len) {
    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1)); // reallocate larger row array

    if (at != E.numrows) {
        memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));
        for (int j = at + 1; j <= E.numrows; j++)
            E.row[j].idx++;
    }

    E.row[at].idx = at;

    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].wraps = 0;
    E.row[at].wrap_stops = NULL;
    E.row[at].render = NULL;
    E.row[at].hl = NULL;
    E.row[at].bg = NULL;
    E.row[at].hl_open_comment = 0;
    editorUpdateRow(&E.row[at]);

    E.numrows++;

    int linenum_w = floor(log10(abs(E.numrows))) + 2;
    if (E.linenum_w < linenum_w) {
        E.linenum_w = linenum_w;
        E.editcols = E.screencols - E.linenum_w;
    }

    E.dirty++;
}

void editorFreeRow(erow *row) {
    free(row->render);
    free(row->chars);
    free(row->hl);
}

void editorDelRow(int at) {
    if (at < 0 || at >= E.numrows)
        return;
    editorFreeRow(&E.row[at]);
    memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1)); // memmove all the remaining rows one up
    for (int j = at; j <= E.numrows - 1; j++)
        E.row[j].idx--;
    E.numrows--;

    int linenum_w = floor(log10(abs(E.numrows))) + 2;
    if (E.linenum_w > linenum_w) {
        E.linenum_w = linenum_w;
        E.editcols = E.screencols - E.linenum_w;
    }

    E.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c) {
    if (at < 0 || at > row->size)
        at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1); // move leftover row to make place for new char
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
    if (at < 0 || at >= row->size)
        return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at); // move the rest of the row and override the deleted char
    row->size--;
    editorUpdateRow(row);
    E.dirty++;
}

/** append buffer ***/

void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b, ab->len + len);
    // char *new = realloc(ab->b, sizeof(ab->b) + len); // temp_fix

    if (new == NULL)
        return;

    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab) {
    free(ab->b);
}