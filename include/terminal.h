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

/**
 * @brief recalculates the current ix (x axis offsets
 *        by wraps) by summing the wrapstops in the
 *        current row
 *        Does not set E.ix directly but returns the
 *        calculated value.
 * 
 * @return int 
 */
int recalcIx();

/**
 * @brief recalculates the current iy (y axis offsets
 *        by wraps) by counting the wraps in the
 *        previously displayed rows.
 *        Does not set E.iy directly but returns the
 *        calculated value.
 * 
 * @return int 
 */
int recalcIy();

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
 * @brief Presents a prompt in the status bar, and lets the user input a
 *        line of text after the prompt.
 *        User is expected to free the return buffer.
 * 
 * @param prompt 
 * @paramt callback a function that will be called after each each keypress.
 *                  the function should accept a char * (input so far),
 *                  and an int (current keypress).
 * @return char* user input
 */
char *editorPrompt(char *prompt, void (*callback)(char *, int));

/**
 * @brief Handle arrow keypresses and move cursor accordingly
 * 
 * @param key
 */
void editorMoveCursor(int key);

/**
 * @brief Moves the cursor $steps times in $key direction.
 *        Can handle negative steps (moves in opposite direction))
 * 
 * @param key arrow key (ARROW_LEFT, etc.)
 * @param steps number of steps to move
 */
void editorStepCursor(int key, int steps);

/**
 * @brief Handle user keypresses (after low level processing)
 * 
 */
void editorProcessKeypress();

#endif