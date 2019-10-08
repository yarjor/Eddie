#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "buffer.h"
#include "consts.h"
#include "editor.h"
#include "file.h"
#include "highlight.h"
#include "search.h"

/*** terminal ***/

void die(const char *s) {
    write(STDOUT_FILENO, ANSI_CLEAR_SCREEN, 4); // clear screen
    write(STDOUT_FILENO, ANSI_HOME_CURSOR, 3);  // home cursor

    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;
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

    if (c == ESCAPE) { // process escape control characters
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

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, ANSI_CURSOR_FORWARD(999) ANSI_CURSOR_DOWN(999), 12) != 12)
            return -1;
        return getCursorPosition(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*** output ***/

void editorScroll() {
    E.rx = 0;
    if (E.cy < E.numrows) {
        E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);
    }

    if (E.cy < E.rowoff) {
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.editrows) {
        E.rowoff = E.cy - E.editrows + 1;
    }
    if (E.rx < E.coloff) {
        E.coloff = E.rx;
    }
    if (E.rx >= E.coloff + E.editcols) {
        E.coloff = E.rx - E.editcols + 1;
    }
}

void editorDrawRows(struct abuf *ab) {
    int y;
    for (y = 0; y < E.editrows; y++) {
        int filerow = y + E.rowoff;
        if (filerow >= E.numrows) {
            if (E.numrows == 0 && y == E.editrows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "Eddie editor -- version %s", EDDIE_VERSION);
                if (welcomelen > E.editcols)
                    welcomelen = E.editcols;
                int padding = (E.editcols - welcomelen) / 2;
                if (padding) {
                    abAppend(ab, "~", 1);
                    padding--;
                }
                while (padding--)
                    abAppend(ab, " ", 1);
                abAppend(ab, welcome, welcomelen);
            } else {
                abAppend(ab, "~", 1);
            }
        } else {
            int len = E.row[filerow].rsize - E.coloff;
            if (len < 0)
                len = 0;
            if (len > E.editcols)
                len = E.editcols;

            abAppend(ab, LINENUM_STYLE_ON, strlen(LINENUM_STYLE_ON));
            char buf[E.linenum_w + 1];
            snprintf(buf, sizeof(buf), "%*d", E.linenum_w - 1, E.row[filerow].idx + 1);
            abAppend(ab, buf, strlen(buf));
            abAppend(ab, LINENUM_STYLE_OFF " ", strlen(LINENUM_STYLE_OFF) + 1);

            char *content = &E.row[filerow].render[E.coloff];
            unsigned char *hl = &E.row[filerow].hl[E.coloff];
            int current_color = -1;
            int j;
            for (j = 0; j < len; j++) {
                if (current_color == STYLE_BRIGHT_BLACK_BG && hl[j] != HL_MATCH) {
                    abAppend(ab, ANSI_STYLE(STYLE_DEFAULT_BG), 5);
                }
                if (iscntrl(content[j])) {
                    char symbol = (content[j] <= 26) ? '@' + content[j] : '?';
                    abAppend(ab, ANSI_REVERSE_VIDEO, 4);
                    abAppend(ab, &symbol, 1);
                    abAppend(ab, ANSI_CLEAR_ATTR, 3);
                    if (current_color != -1) {
                        char buf[16];
                        int clen = snprintf(buf, sizeof(buf), ANSI_STYLE_FMT, current_color);
                        abAppend(ab, buf, clen);
                    }
                } else if (hl[j] == HL_NORMAL) {
                    if (current_color != -1) {
                        abAppend(ab, ANSI_STYLE(STYLE_DEFAULT_FG), 5);
                        current_color = -1;
                    }
                    abAppend(ab, &content[j], 1);
                } else {
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
            abAppend(ab, ANSI_STYLE_DEFAULT_BOTH, 10);
            // abAppend(ab, &E.row[filerow].render[E.coloff], len);
        }

        abAppend(ab, ANSI_ERASE_TO_RIGHT, 3);
        abAppend(ab, "\r\n", 2);
    }
}

void editorDrawStatusBar(struct abuf *ab) {
    abAppend(ab, ANSI_REVERSE_VIDEO, 4);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
                       E.filename ? E.filename : "[No Name]", E.numrows,
                       E.dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d",
                        E.syntax ? E.syntax->filetype : "plaintext",
                        E.cy + 1, E.numrows);
    if (len > E.screencols)
        len = E.screencols;
    abAppend(ab, status, len);
    while (len < E.screencols) { // align rstatus to the right
        if (E.screencols - len == rlen) {
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

void editorDrawMessageBar(struct abuf *ab) {
    abAppend(ab, ANSI_ERASE_TO_RIGHT, 3);
    int msglen = strlen(E.statusmsg);
    if (msglen > E.screencols)
        msglen = E.screencols;
    if (msglen && time(NULL) - E.statusmsg_time < MSG_TIMEOUT)
        abAppend(ab, E.statusmsg, msglen);
}

void editorRefreshScreen() {
    editorScroll();

    struct abuf ab = ABUF_INIT;

    abAppend(&ab, ANSI_HIDE_CURSOR, 6);
    abAppend(&ab, ANSI_HOME_CURSOR, 3);

    editorDrawRows(&ab);
    editorDrawStatusBar(&ab);
    editorDrawMessageBar(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), ANSI_CURSOR_POS_FMT, (E.cy - E.rowoff) + 1, (E.rx - E.coloff + E.linenum_w) + 1);
    abAppend(&ab, buf, strlen(buf));

    abAppend(&ab, ANSI_SHOW_CURSOR, 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
    va_end(ap);
    E.statusmsg_time = time(NULL);
}

/*** input ***/

char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while (1) {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();

        int c = editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
            if (buflen != 0)
                buf[--buflen] = '\0';
        } else if (c == ESCAPE) {
            editorSetStatusMessage("");
            if (callback)
                callback(buf, c);
            free(buf);
            return NULL;
        } else if (c == '\r') {
            if (buflen != 0) {
                editorSetStatusMessage("");
                if (callback)
                    callback(buf, c);
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
            callback(buf, c);
    }
}

void editorMoveCursor(int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key) {
    case ARROW_LEFT:
        if (E.cx != 0) {
            E.cx--;
        } else if (E.cy > 0) {
            E.cy--;
            E.cx = E.row[E.cy].size;
        }
        break;
    case ARROW_RIGHT:
        if (row && E.cx < row->size) {
            E.cx++;
        } else if (row && E.cx == row->size) {
            E.cy++;
            E.cx = 0;
        }
        break;
    case ARROW_UP:
        if (E.cy != 0) {
            E.cy--;
        }
        break;
    case ARROW_DOWN:
        if (E.cy < E.numrows) {
            E.cy++;
        }
        break;
    }

    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen) {
        E.cx = rowlen;
    }
}

void editorProcessKeypress() {
    static int quit_times = EDDIE_QUIT_TIMES;

    int c = editorReadKey();

    switch (c) {
    case '\r': // ENTER
        editorInsertNewLine();
        break;

    case CTRL_KEY('q'):
        if (E.dirty && quit_times > 0) {
            editorSetStatusMessage("File has unsaved hanges. Press Ctrl-Q %d more times to quit.", quit_times);
            quit_times--;
            return;
        }
        write(STDOUT_FILENO, ANSI_CLEAR_SCREEN, 4);
        write(STDOUT_FILENO, ANSI_HOME_CURSOR, 3);
        exit(0);
        break;

    case CTRL_KEY('s'):
        editorSave();
        break;

    case CTRL_KEY('f'):
#ifdef VSCODE
    case CTRL_KEY('r'): // hack for testing in vscode
#endif
        editorFind();
        break;

    case HOME_KEY:
        E.cx = 0;
        break;

    case END_KEY:
        if (E.cy < E.numrows)
            E.cx = E.row[E.cy].size;
        break;

    case BACKSPACE:
    case CTRL_KEY('h'): // legacy backspace
    case DEL_KEY:
        if (c == DEL_KEY)
            editorMoveCursor(ARROW_RIGHT);
        editorDelChar();
        break;

    case PAGE_UP:
    case PAGE_DOWN: {
        if (c == PAGE_UP) {
            E.cy = E.rowoff;
        } else if (c == PAGE_DOWN) {
            E.cy = E.rowoff + E.editrows - 1;
            if (E.cy > E.numrows)
                E.cy = E.numrows;
        }

        int times = E.editrows;
        while (times--)
            editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    } break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        editorMoveCursor(c);
        break;

    case CTRL_KEY('l'): // traditional screen refresh
    case ESCAPE:
        break;

    default:
        editorInsertChar(c);
        break;
    }

    quit_times = EDDIE_QUIT_TIMES;
}