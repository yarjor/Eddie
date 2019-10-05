#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

/*** Highlight Flags **/
#define HL_HIGHLIGHT_NUMBERS (1 << 0)

/**
 * @brief character/string types for syntax highlighting
 * 
 */
enum editorHighlight {
    HL_NORMAL = 0,
    HL_NUMBER,
    HL_MATCH
};

/**
 * @brief Checks whether character c is a separator character.
 * 
 * @param c 
 * @return int 
 */
int is_separator(int c);

/**
 * @brief Analyze a row types and set the matching highlight values in
 *        its hl array.
 * 
 * @param row 
 */
void editorUpdateSyntax(erow *row);

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
void editorSelectSyntaxHighlight();

#endif