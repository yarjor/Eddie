#include "consts.h"
#include "editor.h"
#include "file.h"
#include "terminal.h"

void initEditor() {
    E.cx = 0;
    E.cy = 0;
    E.rx = 0;
    E.ix = 0;
    E.iy = 0;
    E.wrapoff = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.linenum_w = 2;
    E.numrows = 0;
    E.row = NULL;
    E.dirty = 0;
    E.filename = NULL;
    E.statusmsg[0] = '\0';
    E.statusmsg_time = 0;
    E.syntax = NULL;

    if (getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("getWindowSize");
    E.editrows = E.screenrows - 2;           // Make place for status bar and messages
    E.editcols = E.screencols - E.linenum_w; // Make place for line numbers
}

int main(int argc, char *argv[]) {
    enableRawMode();
    initEditor();
    if (argc >= 2) {
        editorOpen(argv[1]);
    }

    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}
