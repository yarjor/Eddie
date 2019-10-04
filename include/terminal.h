#ifndef TERMINAL_H
#define TERMINAL_H

#include "buffer.h"

/*** terminal **/

void die(const char *s);

void disableRawMode();

void enableRawMode();

int editorReadKey();

int getCursorPosition(int *rows, int *cols);

int getWindowSize(int *rows, int *cols);

/*** output ***/

void editorScroll();

void editorDrawRows(struct abuf *ab);

void editorDrawStatusBar(struct abuf *ab);

void editorDrawMessageBar(struct abuf *ab);

void editorRefreshScreen();

void editorSetStatusMessage(const char *fmt, ...);

/*** input ***/

void editorMoveCursor(int key);

void editorProcessKeypress();

#endif