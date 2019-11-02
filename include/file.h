#ifndef FILE_H
#define FILE_H

#include "structs.h"

/**
 * @brief open filename and display in the editor
 * 
 * @param state (the editor state object)
 * @param filename
 */
void editorOpen(eState *estate, char *filename);

/**
 * @brief Save the current editor content to the opened file
 * 
 * @param state (the editor state object)
 */
void editorSave(eState *state);

#endif
