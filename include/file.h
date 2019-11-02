#ifndef FILE_H
#define FILE_H

#include "structs.h"

/**
 * @brief Counts the number of lines in the file.
 *        The function assumes the pointer is to
 *        beginning of file, and will rewind to the
 *        beginning before returning.
 * 
 * @param fp 
 * @return int 
 */

int count_lines(FILE *fp);

/**
 * @brief open filename and display in the editor
 * 
 * @param state (the editor state object)
 * @param filename
 */
void editorOpen(eState *estate, char *filename);

/**
 * @brief Convert the editor's erow array into a single string ready for 
 *        writing to a file.
 * 
 * @param buflen 
 * @param numrows (number of rows being converted)
 * @param rows (pointer to the editor row array)
 * @return char* 
 */
char *editorRowsToString(int *buflen, int numrows, erow *rows);

/**
 * @brief Save the current editor content to the opened file
 * 
 * @param state (the editor state object)
 */
void editorSave(eState *state);

#endif
