#ifndef FILE_H
#define FILE_H

#include "structs.h"

/**
 * @brief open filename and display it in the editor
 * 
 * @param state (pointer to the editor state object)
 * @param filename (path to the file to open)
 */
void editorOpen(eState *estate, char *filename);

/**
 * @brief Save the current open file in place. If no file was open,
 *        prompts the user to "save as".
 * 
 * @param state (pointer to the editor state object)
 */
void editorSave(eState *state);

#endif
