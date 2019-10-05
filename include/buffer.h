#ifndef BUFFER_H
#define BUFFER_H

#include "consts.h"

/*** row operations ***/

/**
 * @brief Convert cursor actual location on row to rendered location
 * 
 * @param row row cursor is rendered on
 * @param cx cursor actual location
 * @return int rendered location
 */
int editorRowCxToRx(erow *row, int cx);

/**
 * @brief Convert cursor rendered location on row to actual location
 * 
 * @param row row cursor is rendered on
 * @param rx cursor rendered location
 * @return int actual location
 */
int editorRowRxToCx(erow *row, int rx);

/**
 * @brief render tabs in a row as spaces
 * 
 * @param row
 */
void editorUpdateRow(erow *row);

/**
 * @brief Insert a new row to the end of the editor's buffer
 * 
 * @param s string with the contents of the row
 * @param len
 */
void editorInsertRow(int at, char *s, size_t len);

/**
 * @brief Frees the memory used by a row (both rendered and actual string)
 * 
 * @param row 
 */
void editorFreeRow(erow *row);

/**
 * @brief deletes the row in index $at.
 * 
 * @param at 
 */
void editorDelRow(int at);

/**
 * @brief Insert a character into an existing row buffer.
 *        If the location is larger than the size of the row,
 *        the character will be appended at the end of the row.
 * 
 * @param row 
 * @param at character insert location
 * @param c the inserted location
 */
void editorRowInsertChar(erow *row, int at, int c);

/**
 * @brief appends a full string (s) to a row.
 * 
 * @param row 
 * @param s 
 * @param len 
 */
void editorRowAppendString(erow *row, char *s, size_t len);

/**
 * @brief Delete a character from the row buffer in column at.
 * 
 * @param row 
 * @param at 
 */
void editorRowDelChar(erow *row, int at);

/*** append buffer ***/

/**
 * @brief appendable char buffer
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
 * @param ab 
 * @param s 
 * @param len 
 */
void abAppend(struct abuf *ab, const char *s, int len);

/**
 * @brief free an append buffer (frees the char*)
 * 
 * @param ab 
 */
void abFree(struct abuf *ab);

#endif