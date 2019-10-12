#include <stdlib.h>
#include <string.h>

#include "highlight.h"
#include "terminal.h"

void editorFindCallback(char *query, int key) {
    static int last_match = -1;
    static int direction = 1;

    if (last_match != -1) {
        memset(E.row[last_match].bg, BG_NORMAL, E.row[last_match].rsize);
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
    for (i = 0; i < E.numrows; i++) {
        current += direction;
        if (current == -1)
            current = E.numrows - 1;
        else if (current == E.numrows)
            current = 0;

        erow *row = &E.row[current];
        char *match = strstr(row->render, query);
        if (match) {
            last_match = current;
            editorStepCursor(ARROW_DOWN, current - E.cy);
            int match_cx = editorRowRxToCx(row, match - row->render);
            editorStepCursor(ARROW_RIGHT, match_cx - E.cx);
            E.rowoff = E.numrows;

            memset(&row->bg[match - row->render], BG_MATCH, strlen(query));
            break;
        }
    }
}

void editorFind() {
    char *query = editorPrompt("Search: %s (User ESC/Arrows/Enter)", editorFindCallback);

    if (query)
        free(query);
}