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
 * @brief render tabs in a row as spaces
 * 
 * @param row
 */
void editorUpdateRow(erow *row);

/**
 * @brief Append a new row to the end of the editor's buffer
 * 
 * @param s string with the contents of the row
 * @param len
 */
void editorAppendRow(char *s, size_t len);

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

/*** append buffer ***/

/**
 * @brief appendable char buffer
 * 
 */
struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT {NULL, 0}

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