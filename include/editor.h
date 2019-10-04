#ifndef EDITOR_H
#define EDITOR_H

/*** editor operations ***/

/**
 * @brief Insert a character (c) in the current cursor location
 * 
 * @param c 
 */
void editorInsertChar(int c);

/**
 * @brief Insert a newline (\n) in the current cursor location,
 *        effectively splitting the current row.
 * 
 */
void editorInsertNewLine();

/**
 * @brief Delete one character in the current cursor location
 * 
 */
void editorDelChar();

#endif