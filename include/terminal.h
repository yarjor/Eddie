#ifndef TERMINAL_H
#define TERMINAL_H

#include "buffer.h"

/*** terminal **/

/**
 * @brief clear screen and return cursor, then print error
 *        message and exit with 1 return
 * 
 * @param s additional error string
 */
void die(const char *s);

/**
 * @brief return to original terminal settings and flags
 *        saved before editor init
 * 
 */
void disableRawMode();

/**
 * @brief Save current terminal config and enter raw mode
 * 
 */
void enableRawMode();

/**
 * @brief user key input processing function (processes escape sequences)
 * 
 * @return int the processed keypress
 */
int editorReadKey();

/**
 * @brief Gets the current cursor position on the screen (not relative to the file)
 * 
 * @param rows row location will be stored here
 * @param cols column location will be stored here
 * @return int return code
 */
int getCursorPosition(int *rows, int *cols);

/**
 * @brief Get the Window Size (by cornering the cursor)
 * 
 * @param rows target row count
 * @param cols target column count
 * @return int status code
 */
int getWindowSize(int *rows, int *cols);

/*** output ***/

/**
 * @brief Scroll the editor by changing the column or row
 *        offset according to the cursor current location
 * 
 */
void editorScroll();

/**
 * @brief Add the editor's file content / empty file display to an appendable buffer.
 *        rows are taken from the global editor state struct.
 * 
 * @param ab the target buffer
 */
void editorDrawRows(struct abuf *ab);

/**
 * @brief Create a status bar with the current filename,
 *        rowcount, and current row.
 * 
 * @param ab the target appendable buffer
 */
void editorDrawStatusBar(struct abuf *ab);

/**
 * @brief Create a bar with brief message. The message
 *        will only be displayed for MSG_TIMEOUT seconds. 
 * 
 * @param ab  the target appendable buffer
 */
void editorDrawMessageBar(struct abuf *ab);

/**
 * @brief Redraw the screen according to the current state:
 *        scroll, update status and message bars, and draw
 *        editor displayed content.
 *        This is the function that actually writes
 *        the content to the terminal.
 * 
 */
void editorRefreshScreen();

/**
 * @brief Show a brief status message for MSG_TIMEOUT seconds
 * 
 * @param fmt format string for the status message
 * @param ... format string variables
 */
void editorSetStatusMessage(const char *fmt, ...);

/*** input ***/

/**
 * @brief Handle arrow keypresses and move cursor accordingly
 * 
 * @param key
 */
void editorMoveCursor(int key);

/**
 * @brief Handle user keypresses (after low level processing)
 * 
 */
void editorProcessKeypress();

#endif