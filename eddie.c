#include "syshead.h"

#include "eddie.h"
#include "consts.h"
#include "editor.h"
#include "file.h"
#include "terminal.h"

eState *initEditor() {
    eState *state = malloc(sizeof(eState));
    state->cx = 0;
    state->cy = 0;
    state->rx = 0;
    state->ix = 0;
    state->iy = 0;
    state->wrapoff = 0;
    state->rowoff = 0;
    state->coloff = 0;
    state->linenum_w = 2;
    state->numrows = 0;
    state->row = NULL;
    state->dirty = 0;
    state->filename = NULL;
    state->statusmsg[0] = '\0';
    state->statusmsg_time = 0;
    state->syntax = NULL;

    if (getWindowSize(&state->screenrows, &state->screencols) == -1)
        die("getWindowSize");
    state->editrows = state->screenrows - 2;                // Make place for status bar and messages
    state->editcols = state->screencols - state->linenum_w; // Make place for line numbers

    return state;
}

int main(int argc, char *argv[]) {
    eState *state;

    enableRawMode();
    state = initEditor();
    if (argc >= 2) {
        editorOpen(state, argv[1]);
    }

    editorSetStatusMessage(state, "HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

    while (1) {
        editorRefreshScreen(state);
        editorProcessKeypress(state);
    }

    return 0;
}
