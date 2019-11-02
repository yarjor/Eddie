#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "structs.h"

/*** Highlight Flags **/
#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)
#define HL_HIGHLIGHT_HASHTAG (1 << 2) // highlight text prefixed with #hashtag
#define HL_HIGHLIGHT_LTGT    (1 << 3) // highlight text between <ltgt>

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
    BG_MATCH, // search highlighted match
};

/**
 * @brief Analyze a row types and set the matching background values in
 *        its bg array.
 * 
 * @param row 
 */
void editorUpdateSyntaxBackground(erow *row);

/**
 * @brief Analyze a row types and set the matching foreground values in
 *        its hl array.
 * 
 * @param state (pointer to the editor state object)
 * @param row 
 */
void editorUpdateSyntaxForeground(eState *state, erow *row);

/**
 * @brief Analyze a row types and set the matching highlight values in
 *        its hl and bg arrays.
 * 
 * @param state (pointer to the editor state object)
 * @param row 
 */
void editorUpdateSyntax(eState *state, erow *row);

/**
 * @brief Matches an ansi style code to the highlight value
 * 
 * @param hl (eddie highlight code)
 * @return int (matched ansi style number)
 */
int editorSyntaxToColor(int hl);

/**
 * @brief Choose and set syntax highlight scheme according to the current filename.
 * 
 * @param state (pointer to the editor state object)
 */
void editorSelectSyntaxHighlight(eState *state);

#endif