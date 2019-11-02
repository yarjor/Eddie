#include "syshead.h"

#include "terminal.h"
#include "buffer.h"
#include "consts.h"
#include "editor.h"
#include "file.h"
#include "highlight.h"
#include "search.h"

/*** terminal ***/

static struct termios orig_termios; // original terminal config storage

void die(const char *s) {
    write(STDOUT_FILENO, ANSI_CLEAR_SCREEN, 4); // clear screen
    write(STDOUT_FILENO, ANSI_HOME_CURSOR, 3);  // home cursor

    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
    }
    printf("\n");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    // Flipping bits by AND-ing with bitwise-NOT of flags
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;  // minimum bytes before read can return
    raw.c_cc[VTIME] = 1; // read timeout

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    if (c == ESCAPE) { // process escape control characters and convert to key codes
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return ESCAPE;
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return ESCAPE;

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return ESCAPE;
                if (seq[2] == '~') {
                    switch (seq[1]) {
                    case '1':
                        return HOME_KEY;
                    case '3':
                        return DEL_KEY;
                    case '4':
                        return END_KEY;
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '7':
                        return HOME_KEY;
                    case '8':
                        return END_KEY;
                    }
                }
            } else {
                switch (seq[1]) {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME_KEY;
                case 'F':
                    return END_KEY;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
            case 'H':
                return HOME_KEY;
            case 'F':
                return END_KEY;
            }
        }

        return ESCAPE;
    } else {
        return c;
    }
}

int getCursorPosition(int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, ANSI_GET_CURSOR_POSITION, 4) != 4) // cursor status query
        return -1;

    // parse status query response
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R') // location sequence beginning
            break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != ESCAPE || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    // try to get window size using ioctl
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // In case gwinsz ioctl fails, use alternate method - 
        // move cursor 999 steps forward and downwards to reach the right bottom corner, and
        // use the cursor location as measurement of the window.
        if (write(STDOUT_FILENO, ANSI_CURSOR_FORWARD(999) ANSI_CURSOR_DOWN(999), 12) != 12) 
            return -1;
        return getCursorPosition(rows, cols);
    } else { // get dimensions from winsize returned by ioctl
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

int recalcIy(eState *state) {
    int y;
    int iy = state->wrapoff; // offset into current row
    // add the number of wraps in all previously displayed rows
    for (y = state->rowoff; y < state->cy; y++) {
        if (y >= state->numrows || y - state->rowoff >= state->editrows)
            return iy;
        iy += state->row[y].wraps;
    }
    return iy;
}

int recalcIx(eState *state) {
    if (state->cy >= state->numrows)
        return 0;
    int ix = 0;
    int cx = state->cx;
    int i;
    erow *row = &state->row[state->cy];
    for (i = 0; i <= row->wraps; i++) {
        if (cx > row->wrap_stops[i]) {
            cx -= row->wrap_stops[i];
            ix -= row->wrap_stops[i];
        }
    }
    return ix;
}

/*** output ***/

void editorScroll(eState *state) {
    state->rx = 0;
    if (state->cy < state->numrows) {
        state->rx = editorRowCxToRx(&state->row[state->cy], state->cx); // calculate rx from cx, if not on last row
    }

    if (state->cy < state->rowoff) { // location is higher than screen, scroll up
        state->rowoff = state->cy;
        state->iy = 0; // the cursor is on the top row
    }
    if (state->cy + state->iy >= state->rowoff + state->editrows) { // location is lower than screen, scroll down
        state->rowoff = state->cy + state->iy - state->editrows + 1;
        state->iy = recalcIy(state); // wrapped rows were possibly scrolled over, recalculate iy in case it changed
    }
#ifndef DO_SOFTWRAP
    if (state->rx < state->coloff) { // location is left of the screen, scroll left
        state->coloff = state->rx;
    }
    if (state->rx >= state->coloff + state->editcols) { // location is right of the screen, scroll right
        state->coloff = state->rx - state->editcols + 1;
    }
#endif /* DO_SOFTWRAP */
}

void editorDrawRows(eState *state, struct abuf *ab) {
    int y;
    int show_rows = state->editrows;
    for (y = 0; y < show_rows; y++) {
        int filerow = y + state->rowoff;
        if (filerow >= state->numrows) { // rendering an empty part of the editor
            if (state->numrows == 0 && y == state->editrows / 3) { // editor is completely empty - show welcome message
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "Eddie editor -- version %s", EDDIE_VERSION);
                if (welcomelen > state->editcols)
                    welcomelen = state->editcols;
                int padding = (state->editcols - welcomelen) / 2;
                if (padding) {
                    abAppend(ab, "~", 1);
                    padding--;
                }
                while (padding--)
                    abAppend(ab, " ", 1);
                abAppend(ab, welcome, welcomelen);
            } else { // show ~ on line beginnings
                abAppend(ab, "~", 1);
            }
        } else {
            int len = state->row[filerow].rsize;
#ifndef DO_SOFTWRAP
            len -= state->coloff;
#endif /* DO_SOFTWRAP */
            if (len < 0)
                len = 0;
#ifndef DO_SOFTWRAP
            if (len > state->editcols)
                len = state->editcols;
#endif /* DO_SOFTWRAP */

            // create line numbering column
            abAppend(ab, LINENUM_STYLE_ON, strlen(LINENUM_STYLE_ON));
            char buf[state->linenum_w + 1];
            snprintf(buf, sizeof(buf), "%*d", state->linenum_w - 1, state->row[filerow].idx + 1);
            abAppend(ab, buf, strlen(buf));
            abAppend(ab, LINENUM_STYLE_OFF " ", strlen(LINENUM_STYLE_OFF) + 1);

            char *content = state->row[filerow].render;
            unsigned char *hl = state->row[filerow].hl;
            unsigned char *bg = state->row[filerow].bg;
#ifndef DO_SOFTWRAP // if no softwrap, position properly in arrays
            content = &content[state->coloff];
            hl = &hl[state->coloff];
            bg = &bg[state->coloff];
#endif /* DO_SOFTWRAP */
            int current_color = -1;
            int current_bgcolor = -1;
            int j;

            for (j = 0; j < len; j++) {
                int bgcolor = editorSyntaxToColor(bg[j]);
                if (bgcolor != current_bgcolor) { // change background style
                    current_bgcolor = bgcolor;
                    char buf[16];
                    int clen = snprintf(buf, sizeof(buf), ANSI_STYLE_FMT, bgcolor);
                    abAppend(ab, buf, clen);
                }
                if (content[j] == '\n') { // reached a wrap point
#ifdef DO_SOFTWRAP
                    if (y >= show_rows - 1)
                        break; // when wrapped row goes past the end of the screen, stop printing it
                    abAppend(ab, ANSI_ERASE_TO_RIGHT, 3);
                    abAppend(ab, "\r\n", 2); // actual wrap
                    show_rows--; // wrapped row takes one more screen row
                    abAppend(ab, LINENUM_STYLE_ON, strlen(LINENUM_STYLE_ON));
                    char buf[state->linenum_w + 1];
                    snprintf(buf, sizeof(buf), "%*c", state->linenum_w - 1, ' '); // create an empty numbering column (for margin & style)
                    abAppend(ab, buf, strlen(buf));
                    abAppend(ab, LINENUM_STYLE_OFF " ", strlen(LINENUM_STYLE_OFF) + 1);
#endif /* DO_SOFTWRAP */
                } else if (iscntrl(content[j])) { // special print for control characters (reverse video and marking)
                    char symbol = (content[j] <= 26) ? '@' + content[j] : '?';
                    abAppend(ab, ANSI_REVERSE_VIDEO, 4);
                    abAppend(ab, &symbol, 1);
                    abAppend(ab, ANSI_CLEAR_ATTR, 3);
                    if (current_color != -1) {
                        char buf[16];
                        int clen = snprintf(buf, sizeof(buf), ANSI_STYLE_FMT, current_color); // return to the current format after clear attr was used
                        abAppend(ab, buf, clen);
                    }
                } else if (hl[j] == HL_NORMAL) { // normal highlight - reset color
                    if (current_color != -1) {
                        abAppend(ab, ANSI_STYLE(STYLE_DEFAULT_FG), 5);
                        current_color = -1;
                    }
                    abAppend(ab, &content[j], 1);
                } else { // special foreground highlight
                    int color = editorSyntaxToColor(hl[j]);
                    if (color != current_color) {
                        current_color = color;
                        char buf[16];
                        int clen = snprintf(buf, sizeof(buf), ANSI_STYLE_FMT, color);
                        abAppend(ab, buf, clen);
                    }
                    abAppend(ab, &content[j], 1);
                }
            }
            abAppend(ab, ANSI_STYLE_DEFAULT_BOTH, 10); // reset background and foreground before next row
        }

        abAppend(ab, ANSI_ERASE_TO_RIGHT, 3);
        abAppend(ab, "\r\n", 2);
    }
}

void editorDrawStatusBar(eState *state, struct abuf *ab) {
    abAppend(ab, ANSI_REVERSE_VIDEO, 4);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
                       state->filename ? state->filename : "[No Name]", state->numrows,
                       state->dirty ? "(modified)" : ""); // {filename} - {count} lines (modified)
    int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d",
                        state->syntax ? state->syntax->filetype : "plaintext",
                        state->cy + 1, state->numrows); // {syntax} | {curline}/{countlines}
    if (len > state->screencols)
        len = state->screencols;
    abAppend(ab, status, len);
    while (len < state->screencols) { // align rstatus to the right
        if (state->screencols - len == rlen) {
            abAppend(ab, rstatus, rlen);
            break;
        } else {
            abAppend(ab, " ", 1);
            len++;
        }
    }
    abAppend(ab, ANSI_CLEAR_ATTR, 3);
    abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(eState *state, struct abuf *ab) {
    abAppend(ab, ANSI_ERASE_TO_RIGHT, 3);
    int msglen = strlen(state->statusmsg);
    if (msglen > state->screencols)
        msglen = state->screencols;
    if (msglen && time(NULL) - state->statusmsg_time < MSG_TIMEOUT)
        abAppend(ab, state->statusmsg, msglen);
}

void editorRefreshScreen(eState *state) {
    editorScroll(state);

    struct abuf ab = ABUF_INIT;

    abAppend(&ab, ANSI_HIDE_CURSOR, 6);
    abAppend(&ab, ANSI_HOME_CURSOR, 3);

    editorDrawRows(state, &ab);
    editorDrawStatusBar(state, &ab);
    editorDrawMessageBar(state, &ab);

    char buf[32];
#ifdef DO_SOFTWRAP
    snprintf(buf, sizeof(buf), ANSI_CURSOR_POS_FMT, (state->cy - state->rowoff + state->iy) + 1, (state->rx + state->linenum_w + state->ix) + 1);
#else
    snprintf(buf, sizeof(buf), ANSI_CURSOR_POS_FMT, (state->cy - state->rowoff + state->iy) + 1, (state->rx - state->coloff + state->linenum_w + ec->ix) + 1);
#endif /* DO_SOFTWRAP */
    abAppend(&ab, buf, strlen(buf));

    abAppend(&ab, ANSI_SHOW_CURSOR, 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}

void editorSetStatusMessage(eState *state, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(state->statusmsg, STATUS_MSG_LEN, fmt, ap);
    va_end(ap);
    state->statusmsg_time = time(NULL);
}

/*** input ***/

char *editorPrompt(eState *state, char *prompt, void (*callback)(eState *, char *, int)) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while (1) {
        editorSetStatusMessage(state, prompt, buf);
        editorRefreshScreen(state);

        int c = editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) { // deleting keys
            if (buflen != 0)
                buf[--buflen] = '\0';
        } else if (c == ESCAPE) { // escape was pressed - call callback but don't return the input
            editorSetStatusMessage(state, "");
            if (callback)
                callback(state, buf, c);
            free(buf);
            return NULL;
        } else if (c == '\r') { // enter was pressed - callback and return collected input
            if (buflen != 0) {
                editorSetStatusMessage(state, "");
                if (callback)
                    callback(state, buf, c);
                return buf;
            }
        } else if (!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }

        if (callback)
            callback(state, buf, c);
    }
}

void editorMoveCursor(eState *state, int key) {
    erow *row = (state->cy >= state->numrows) ? NULL : &state->row[state->cy]; // current row

    switch (key) {
    case ARROW_LEFT:
        if (state->cx != 0) {
#ifdef DO_SOFTWRAP
            if (state->cx + state->ix == 0 && state->iy > 0) { // Leftmost column reached on a wrapped row - go one wrap up and land on rightmost column
                state->wrapoff -= 1;
                state->ix += state->row[state->cy].wrap_stops[state->wrapoff];
                state->iy -= 1;
            }
#endif /* DO_SOFTWRAP */
            state->cx--;
        } else if (state->cy > 0) { // was on beginning of row - go one row up
            state->cy--;
            state->cx = state->row[state->cy].size;
#ifdef DO_SOFTWRAP
            // add all wraps for previous row to land on end
            state->wrapoff = state->row[state->cy].wraps;
            state->ix = 0;
            for (int wr = 0; wr < state->wrapoff; wr++) { // recalculate ix (maybe can replace by recalcIx)
                state->ix -= state->row[state->cy].wrap_stops[wr];
            }
#endif /* DO_SOFTWRAP */
        }
        break;
    case ARROW_RIGHT:
        if (row && state->cx < row->size) {
#ifdef DO_SOFTWRAP
            int current_wrapstop = state->row[state->cy].wrap_stops[state->wrapoff];
            if (state->cx + state->ix >= current_wrapstop) { // passed a wrap stop, go to beginning of next wrap
                state->ix -= current_wrapstop;
                state->iy += 1;
                state->wrapoff += 1;
            }
#endif /* DO_SOFTWRAP */
            state->cx++;
        } else if (row && state->cx == row->size) { // was on end of row, go to beginning of next row
            state->cy++;
            state->cx = 0;
#ifdef DO_SOFTWRAP
            state->ix = 0;
            state->wrapoff = 0;
#endif /* DO_SOFTWRAP */
        }
        break;
    case ARROW_UP:
        if (state->cy != 0) {
#ifdef DO_SOFTWRAP
            // fix wrapping to match the previous row's length and stops
            state->iy -= state->wrapoff + state->row[state->cy - 1].wraps;
            int cx = state->cx;
            int wo;
            for (wo = 0; wo < state->row[state->cy - 1].wraps; wo++) {
                cx -= state->row[state->cy - 1].wrap_stops[wo];
                if (cx < 0)
                    break;
            }
            state->wrapoff = wo;
            state->iy += state->wrapoff;
#endif /* DO_SOFTWRAP */
            state->cy--;
        }
        break;
    case ARROW_DOWN:
        if (state->cy < state->numrows) {
#ifdef DO_SOFTWRAP
            // fix wrapping to match the next row's length and stops
            state->iy += state->row[state->cy].wraps - state->wrapoff;
            if (state->row[state->cy + 1].wraps < state->wrapoff)
                state->wrapoff = state->row[state->cy + 1].wraps;
            state->iy += state->wrapoff;
            state->ix = 0;
            for (int wr = 0; wr < state->wrapoff; wr++) {
                state->ix -= state->row[state->cy - 1].wrap_stops[wr];
            }
#endif /* DO_SOFTWRAP */
            state->cy++;
        }
        break;
    }

    row = (state->cy >= state->numrows) ? NULL : &state->row[state->cy];
    int rowlen = row ? row->size : 0;
    if (state->cx > rowlen) { // limit cx as it was not limited in the switch case
        state->cx = rowlen;
#ifdef DO_SOFTWRAP
        state->wrapoff = state->row[state->cy].wraps;
        state->ix = 0;
        for (int wr = 0; wr < state->wrapoff; wr++) {
            state->ix -= state->row[state->cy].wrap_stops[wr];
        }
#endif /* DO_SOFTWRAP */
    }
}

void editorStepCursor(eState *state, int key, int steps) {
    if (steps < 0) { // negative steps -> reverse direction and renegate
        switch (key) {
        case ARROW_LEFT:
            key = ARROW_RIGHT;
            break;
        case ARROW_RIGHT:
            key = ARROW_LEFT;
            break;
        case ARROW_UP:
            key = ARROW_DOWN;
            break;
        case ARROW_DOWN:
            key = ARROW_UP;
            break;
        }
        steps = -steps;
    }
    for (int i = 0; i < steps; i++) {
        editorMoveCursor(state, key);
    }
}

void editorProcessKeypress(eState *state) {
    static int quit_times = EDDIE_QUIT_TIMES;

    int c = editorReadKey();

    switch (c) {
    case '\r': // ENTER
        editorInsertNewLine(state);
        break;

    case CTRL_KEY('q'):
        if (state->dirty && quit_times > 0) {
            editorSetStatusMessage(state, "File has unsaved hanges. Press Ctrl-Q %d more times to quit.", quit_times);
            quit_times--;
            return;
        }
        write(STDOUT_FILENO, ANSI_CLEAR_SCREEN, 4);
        write(STDOUT_FILENO, ANSI_HOME_CURSOR, 3);
        exit(0);
        break;

    case CTRL_KEY('s'):
        editorSave(state);
        break;

    case CTRL_KEY('f'):
#ifdef VSCODE
    case CTRL_KEY('r'): // hack for testing in vscode
#endif
        editorFind(state);
        break;

    case HOME_KEY:
        editorStepCursor(state, ARROW_LEFT, state->cx);
        break;

    case END_KEY:
        if (state->cy < state->numrows)
            editorStepCursor(state, ARROW_RIGHT, state->row[state->cy].size - state->cx);
        break;

    case BACKSPACE:
    case CTRL_KEY('h'): // legacy backspace
    case DEL_KEY:
        if (c == DEL_KEY)
            editorMoveCursor(state, ARROW_RIGHT);
        editorDelChar(state);
        break;

    case PAGE_UP:
    case PAGE_DOWN:
        editorStepCursor(state, c == PAGE_UP ? ARROW_UP : ARROW_DOWN, state->editrows);
        state->rowoff = state->numrows; // This will cause the landed row to be on top of the display
        break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        editorMoveCursor(state, c);
        break;

    case CTRL_KEY('l'): // traditional screen refresh
    case ESCAPE:
        break;

    default:
        editorInsertChar(state, c);
        break;
    }

    quit_times = EDDIE_QUIT_TIMES;
}