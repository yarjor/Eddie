#ifndef FILE_H
#define FILE_H

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