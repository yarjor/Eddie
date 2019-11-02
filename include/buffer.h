#ifndef BUFFER_H
#define BUFFER_H

#include "consts.h"

/*** row operations ***/

/**
 * @brief Convert cursor actual location on row to rendered location
 *        (difference is in tab-space conversion)
 * 
 * @param row (row cursor is rendered on)
 * @param cx (cursor actual location to be converted)
 * @return int (cursor rendered location)
 */
int editorRowCxToRx(erow *row, int cx);

/**
 * @brief Convert cursor rendered location on row to actual location
 *        (difference is in tab-space conversion)
 * 
 * @param row (row cursor is rendered on)
 * @param rx (cursor rendered location to be converted)
 * @return int (cursor actual location)
 */
int editorRowRxToCx(erow *row, int rx);

/**
 * @brief Updates a row object with visual render details according
 *        to its chars (actual row string).
 *        Softwrap is calculated (if set), tabs are rendered to spaces
 *        and syntax highlighting is added.
 * 
 * @param state (pointer to the editor state object)
 * @param row (the row being updated)
 */
void editorUpdateRow(eState *state, erow *row);

/**
 * @brief Insert a new row to the editor's buffer.
 * 
 * @param state (pointer to the editor state object)
 * @param at (insert location - row index)
 * @param s (string with the contents of the new row)
 * @param len (length of the new row)
 */
void editorInsertRow(eState *state, int at, char *s, size_t len);


/**
 * @brief deletes the row in index @at and adjusts the row
 *        array accordingly.
 * 
 * @param state (pointer to the editor state object)
 * @param at (index of the row deleted)
 */
void editorDelRow(eState *state, int at);

/**
 * @brief Insert a character into an existing row buffer.
 *        If the location is larger than the size of the row,
 *        the character will be appended at the end of the row.
 * 
 * @param state (pointer to the editor state object)
 * @param row (the row being inserted into)
 * @param at (character insert location in row)
 * @param c (the character inserted)
 */
void editorRowInsertChar(eState *state, erow *row, int at, int c);

/**
 * @brief appends a full string (s) to a row.
 * 
 * @param state (pointer to the editor state object)
 * @param row (the row being inserted into)
 * @param s (string appended to the row)
 * @param len (length of the appended string)
 */
void editorRowAppendString(eState *state, erow *row, char *s, size_t len);

/**
 * @brief Delete a character from the row buffer in column @at.
 * 
 * @param state (pointer to the editor state object)
 * @param row (the row being deleted from)
 * @param at (location of deleted character)
 */
void editorRowDelChar(eState *state, erow *row, int at);

/*** append buffer ***/

/**
 * @brief simple struct for holding an appendable character
 *        buffer.
 * 
 */
struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT \
    { NULL, 0 }

/**
 * @brief Append a string to an append-buffer
 * 
 * @param ab (append-buffer)
 * @param s (appended string)
 * @param len (length of appended string)
 */
void abAppend(struct abuf *ab, const char *s, int len);

/**
 * @brief free an append buffer (frees the char* and sets length to 0, 
 *        does not free the struct)
 * 
 * @param ab (append-buffer)
 */
void abFree(struct abuf *ab);

#endif