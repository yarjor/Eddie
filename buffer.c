#include "syshead.h"

#include "buffer.h"
#include "consts.h"
#include "highlight.h"
#include "structs.h"

/**
 * @brief Calculates character distance from @at to next
 *        space in row / end of row
 * 
 * @param row (relevant erow pointer)
 * @param at (location to start calculating from)
 * @return int (distance to the next space / eor)
 */
static int distance_to_next_space(erow *row, int at) {
    int j;
    for (j = at + 1; j < row->size; j++) {
        if (isspace(row->chars[j]))
            break;
    }
    return j - at;
}

/**
 * @brief Calculates character distance to @at from previous
 *        space in row / start of row
 * 
 * @param row (relevant erow pointer)
 * @param at (location to start calculating from)
 * @return int (distance to the next space / sor)
 */
static int distance_from_prev_space(erow *row, int at) {
    int j;
    for (j = at - 1; j >= 0; j--) {
        if (isspace(row->chars[j]))
            break;
    }
    return at - j;
}

/**
 * @brief Frees the memory used by a row (both rendered and actual string)
 * 
 * @param row 
 */
static void free_row(erow *row) {
    free(row->render);
    free(row->chars);
    free(row->hl);
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

void editorUpdateRow(eState *state, erow *row) {
    // Calculate the number of tabs first, to allocate enoguh memory for render
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t')
            tabs++;

    free(row->render);
    int size = (row->size + tabs * (EDDIE_TAB_STOP - 1)) + 1;
    row->render = malloc(size);
    // wrap stops are not taken into accound when allocating render,
    // which will be reallocated if necessary.

    int idx = 0;
    int row_idx = 0;
    row->wraps = 0;
    row->wrap_stops = malloc(sizeof(int)); // start with single int array
    row->wrap_stops[0] = state->editcols;
    for (j = 0; j < row->size; j++) {
#ifdef DO_SOFTWRAP
        int to_next_space = distance_to_next_space(row, j);
        int from_prev_space = distance_from_prev_space(row, j);
        if ((isspace(row->chars[j]) && // next word will overflow the display
                row_idx + to_next_space >= state->editcols) ||
            (row_idx >= state->editcols && // word is too long to avoid breaking
                from_prev_space >= state->editcols / 2)) {
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
            do {
                row->render[idx++] = ' ';
                row_idx++;
            } while (idx % EDDIE_TAB_STOP != 0);
        } else {
            row->render[idx++] = row->chars[j];
            row_idx++;
        }
    }
#ifdef DO_SOFTWRAP
    row->wrap_stops = realloc(row->wrap_stops, (row->wraps + 1) * sizeof(int));
    row->wrap_stops[row->wraps] = row_idx; // set last wrap stop to end of the row
#endif /* DO_SOFTWRAP */
    row->render[idx] = '\0';
    row->rsize = idx;

    editorUpdateSyntax(state, row);
}

void editorInsertRow(eState *state, int at, char *s, size_t len) {
    state->row = realloc(state->row, sizeof(erow) * (state->numrows + 1)); // reallocate larger row array

    if (at != state->numrows) { 
        // move rows after @at to make place for new row
        memmove(&state->row[at + 1], &state->row[at], sizeof(erow) * (state->numrows - at));
        for (int j = at + 1; j <= state->numrows; j++)
            state->row[j].idx++;
    }

    state->row[at].idx = at;

    state->row[at].size = len;
    state->row[at].chars = malloc(len + 1); // extra character for null-termination
    memcpy(state->row[at].chars, s, len);
    state->row[at].chars[len] = '\0';

    state->row[at].rsize = 0;
    state->row[at].wraps = 0;
    state->row[at].wrap_stops = NULL;
    state->row[at].render = NULL;
    state->row[at].hl = NULL;
    state->row[at].bg = NULL;
    state->row[at].hl_open_comment = 0;
    editorUpdateRow(state, &state->row[at]);

    state->numrows++;

    // recalculate the numbering column width in case the new row caused its width to overflow
    int linenum_w = floor(log10(abs(state->numrows))) + 2;
    if (state->linenum_w < linenum_w) {
        state->linenum_w = linenum_w;
        state->editcols = state->screencols - state->linenum_w;
    }

    state->dirty++;
}

void editorDelRow(eState *state, int at) {
    if (at < 0 || at >= state->numrows)
        return; // illegal delete location
    free_row(&state->row[at]);
    // move all the remaining rows one backwards
    memmove(&state->row[at], &state->row[at + 1], sizeof(erow) * (state->numrows - at - 1));
    for (int j = at; j <= state->numrows - 1; j++)
        state->row[j].idx--;
    state->numrows--;

    // recalculate the numbering column width in case the deleted row allows it to be narrower
    int linenum_w = floor(log10(abs(state->numrows))) + 2;
    if (state->linenum_w > linenum_w) {
        state->linenum_w = linenum_w;
        state->editcols = state->screencols - state->linenum_w;
    }

    state->dirty++;
}

void editorRowInsertChar(eState *state, erow *row, int at, int c) {
    if (at < 0 || at > row->size)
        at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1); // move leftover row to make place for new char
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(state, row);
    state->dirty++;
}

void editorRowAppendString(eState *state, erow *row, char *s, size_t len) {
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(state, row);
    state->dirty++;
}

void editorRowDelChar(eState *state, erow *row, int at) {
    if (at < 0 || at >= row->size)
        return; // illegal delete location
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at); // move the rest of the row and override the deleted char
    row->size--;
    editorUpdateRow(state, row);
    state->dirty++;
}

/** append buffer ***/

void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL)
        return;

    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab) {
    free(ab->b);
    ab->len = 0;
}