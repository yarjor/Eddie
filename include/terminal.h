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
 * @param state (the editor state object)
 * @return int 
 */
int recalcIx(eState *state);

/**
 * @brief recalculates the current iy (y axis offsets
 *        by wraps) by counting the wraps in the
 *        previously displayed rows.
 *        Does not set E.iy directly but returns the
 *        calculated value.
 * 
 * @param state (the editor state object)
 * @return int 
 */
int recalcIy(eState *state);

/*** output ***/

/**
 * @brief Scroll the editor by changing the column or row
 *        offset according to the cursor current location
 * 
 * @param state (the editor state object)
 */
void editorScroll(eState *state);

/**
 * @brief Add the editor's file content / empty file display to an appendable buffer.
 *        rows are taken from the global editor state struct.
 * 
 * @param state (the editor state object)
 * @param ab the target buffer
 */
void editorDrawRows(eState *state, struct abuf *ab);

/**
 * @brief Create a status bar with the current filename,
 *        rowcount, and current row.
 * 
 * @param state (the editor state object)
 * @param ab the target appendable buffer
 */
void editorDrawStatusBar(eState *state, struct abuf *ab);

/**
 * @brief Create a bar with brief message. The message
 *        will only be displayed for MSG_TIMEOUT seconds. 
 * 
 * @param state (the editor state object)
 * @param ab  the target appendable buffer
 */
void editorDrawMessageBar(eState *state, struct abuf *ab);

/**
 * @brief Redraw the screen according to the current state:
 *        scroll, update status and message bars, and draw
 *        editor displayed content.
 *        This is the function that actually writes
 *        the content to the terminal.
 * 
 * @param state (the editor state object)
 */
void editorRefreshScreen(eState *state);

/**
 * @brief Show a brief status message for MSG_TIMEOUT seconds
 * 
 * @param state (the editor state object)
 * @param fmt format string for the status message
 * @param ... format string variables
 */
void editorSetStatusMessage(eState *state, const char *fmt, ...);

/*** input ***/

/**
 * @brief Presents a prompt in the status bar, and lets the user input a
 *        line of text after the prompt.
 *        User is expected to free the return buffer.
 * 
 * @param state (the editor state object)
 * @param prompt 
 * @param callback a function that will be called after each each keypress.
 *                  the function should accept a char * (input so far),
 *                  and an int (current keypress).
 * @return char* user input
 */
char *editorPrompt(eState *state, char *prompt, void (*callback)(eState *, char *, int));

/**
 * @brief Handle arrow keypresses and move cursor accordingly
 * 
 * @param state (the editor state object)
 * @param key
 */
void editorMoveCursor(eState *state, int key);

/**
 * @brief Moves the cursor $steps times in $key direction.
 *        Can handle negative steps (moves in opposite direction))
 * 
 * @param state (the editor state object)
 * @param key arrow key (ARROW_LEFT, etc.)
 * @param steps number of steps to move
 */
void editorStepCursor(eState *state, int key, int steps);

/**
 * @brief Handle user keypresses (after low level processing)
 * 
 * @param state (the editor state object)
 */
void editorProcessKeypress(eState *state);

#endif