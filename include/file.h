#ifndef FILE_H
#define FILE_H

/**
 * @brief Counts the number of lines in the file.
 *        The function assumes the pointer is to
 *        beginning of file, and will rewind to the
 *        beginning before returning.
 * 
 * @param fp 
 * @return int 
 */

int countLines(FILE *fp);

/**
 * @brief open filename and display in the editor
 * 
 * @param filename 
 */
void editorOpen(char *filename);

/**
 * @brief Convert the editor's erow array into a single string ready for 
 *        writing to a file.
 * 
 * @param buflen 
 * @return char* 
 */
char *editorRowsToString(int *buflen);

/**
 * @brief Save the current editor content to the opened file
 * 
 */
void editorSave();

#endif