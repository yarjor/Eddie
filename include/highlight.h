#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "structs.h"

/*** Highlight Flags **/
#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)
#define HL_HIGHLIGHT_HASHTAG (1 << 2)
#define HL_HIGHLIGHT_LTGT    (1 << 3)

#define HASHTAG "#"

/**
 * @brief character/string types for syntax highlighting
 * 
 */
enum editorHighlight {
    HL_NORMAL = 0,
    HL_COMMENT,
    HL_MLCOMMENT,
    HL_KEYWORD1,
    HL_KEYWORD2,
    HL_KEYWORD3,
    HL_HASHTAG,
    HL_LTGT,
    HL_STRING,
    HL_NUMBER,
    BG_NORMAL,
    BG_MATCH,
};

/**
 * @brief Checks whether character c is a separator character.
 * 
 * @param c 
 * @return int 
 */
int is_separator(int c);

/**
 * @brief Analyze a row types and set the matching background values in
 *        its bg array.
 * 
 * @param row 
 */
void editorUpdateSyntaxBackground(erow *row);

/**
 * @brief Analyze a row types and set the matching background values in
 *        its bg array.
 * 
 * @param state (the editor state object)
 * @param row 
 */
void editorUpdateSyntaxForeground(eState *state, erow *row);

/**
 * @brief Analyze a row types and set the matching highlight values in
 *        its hl array.
 * 
 * @param state (the editor state object)
 * @param row 
 */
void editorUpdateSyntax(eState *state, erow *row);

/**
 * @brief Matches ansi style code to the highlight value
 * 
 * @param hl 
 * @return int  
 */
int editorSyntaxToColor(int hl);

/**
 * @brief Choose and set syntax highlight according to the current filename.
 * 
 */
void editorSelectSyntaxHighlight(eState *state);

#endif