#ifndef EDITOR_H
#define EDITOR_H

#include "structs.h"

/*** editor operations ***/

/**
 * @brief Insert a character (c) in the current cursor location
 * 
 * @param state (pointer to the editor state object)
 * @param c (the character to be inserted)
 */
void editorInsertChar(eState *state, int c);

/**
 * @brief Insert a newline (\n) in the current cursor location,
 *        effectively splitting the current row if the cursor is in the middle
 *        of it.
 * 
 * @param state (pointer to the editor state object)
 */
void editorInsertNewLine(eState *state);

/**
 * @brief Delete one character in the current cursor location
 *        (one char backwards - like backspace)
 * 
 * @param state (pointer to the editor state object)
 */
void editorDelChar(eState *state);

#endif