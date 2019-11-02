#include "syshead.h"

#include "search.h"
#include "highlight.h"
#include "terminal.h"

void editorFindCallback(eState *state, char *query, int key) {
    static int last_match = -1;
    static int direction = 1;

    if (last_match != -1) {
        memset(state->row[last_match].bg, BG_NORMAL, state->row[last_match].rsize);
    }

    if (key == '\r' || key == ESCAPE) {
        last_match = -1;
        direction = 1;
        return;
    } else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
        direction = 1;
    } else if (key == ARROW_LEFT || key == ARROW_UP) {
        direction = -1;
    } else {
        last_match = -1;
        direction = 1;
    }

    if (last_match == -1)
        direction = 1;
    int current = last_match;
    int i;
    for (i = 0; i < state->numrows; i++) {
        current += direction;
        if (current == -1)
            current = state->numrows - 1;
        else if (current == state->numrows)
            current = 0;

        erow *row = &state->row[current];
        char *match = strstr(row->render, query);
        if (match) {
            last_match = current;
            editorStepCursor(state, ARROW_DOWN, current - state->cy);
            int match_cx = editorRowRxToCx(row, match - row->render);
            match_cx += strlen(query);
            editorStepCursor(state, ARROW_RIGHT, match_cx - state->cx);
            state->rowoff = state->numrows;
            editorScroll(state);
#ifdef DO_SOFTWRAP
            state->iy = recalcIy(state);
            state->ix = recalcIx(state);
#endif /* DO_SOFTWRAP */

            memset(&row->bg[match - row->render], BG_MATCH, strlen(query));
            break;
        }
    }
}

void editorFind(eState *state) {
    char *query = editorPrompt(state, "Search: %s (User ESC/Arrows/Enter)", editorFindCallback);

    if (query)
        free(query);
}
