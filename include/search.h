#ifndef FIND_H
#define FIND_H

/**
 * @brief Iterates the file rows, moves the cursor to the first 
 *        matching string, and scrolls the chosen row to the top of 
 *        the screen.
 *        Search is case-sensitive.
 * 
 * @param query search query
 * @param key the last key pressed before the call
 */
void editorFindCallback(char *query, int key);

/**
 * @brief prompts the user for a search string and iterates the file
 *        rows, and moves the cursor to the first matching string, and
 *        scrolls the chosen row to the top of the screen.
 *        Search is case-sensitive.
 * 
 */
void editorFind();

#endif