#ifndef STRUCTS_H
#define STRUCTS_H

#include "syshead.h"

#define STATUS_MSG_LEN 80

/**
 * @brief contains the syntax highlighting information for a certain filetype
 * 
 * {
 *      char **filematch; (patterns to match filename against)
 *      char **keywords; (first type of keywords is regular, second type is terminated with a pipe, third with backtick)
 *      char *singleline_comment_start; (string marking the start of single-line comment)
 *      char *multiline_comment_start; (string marking the start of multi-line comment)
 *      char *multiline_comment_end; (string marking the end of multi-line comment)
 *      int flags; (syntax highlighting flags)
 *  }
*/
struct editorSyntax {
    char *filetype;
    char **filematch;
    char **keywords;
    char *singleline_comment_start;
    char *multiline_comment_start;
    char *multiline_comment_end;
    int flags;
};

/**
 * @brief struct for holding a displayed row in the editor.
 * 
 * {
 *      int idx; (index in the file)
 *      int size; (size of chars array)
 *      int rsize; (size of render array)
 *      int wraps; (number of wraps in the row)
 *      int *wrap_stops; (array with the location the row wraps on)
 *      char *chars; (the actual character content of the row)
 *      char *render; (the rendered content of the row)
 *      unsigned char *hl; (foreground syntax highlight code array)
 *      unsigned char *bg; (background syntax highlight code array)
 *      int hl_open_comment; (whether the row has an open multiline comment)
 * }
 */
typedef struct erow {
    int idx;
    int size;
    int rsize;
    int wraps;
    int *wrap_stops;
    char *chars;
    char *render;
    unsigned char *hl;
    unsigned char *bg;
    int hl_open_comment;
} erow;

/**
 * @brief typedef struct holding the state of the editor.
 * {
 *      int cx, cy; (cursor location related to actual file)
 *      int rx; (cursor x location on rendered line)
 *      int ix, iy; (added offsets to x, y because of soft wrap)
 *      int wrapoff; (current wrap offset into the row standing on)
 *      int rowoff; (screen row offset)
 *      int coloff; (screen column offset)
 *      int screenrows; (screen size rows)
 *      int screencols; (screen size columns)
 *      int editrows; (number of rows in the editing window - excluding status bar e.g)
 *      int editcols; (number of columns in the editing window - excluding numbering column e.g)
 *      int linenum_w; (width of numbering column)
 *      int numrows; (number of rows in the file)
 *      erow *row; (pointer to row array)
 *      int dirty; (whether the file was modified since opening)
 *      char *filename; (name of the open file)
 *      char statusmsg[STATUS_MSG_LEN]; (last set status message)
 *      time_t statusmsg_time; (time the status message was last set)
 *      struct editorSyntax *syntax; (syntax highlighting struct)
 *  }
 */
typedef struct editor_state {
    int cx, cy;
    int rx;
    int ix, iy;
    int wrapoff;
    int rowoff;
    int coloff;
    int screenrows;
    int screencols;
    int editrows;
    int editcols;
    int linenum_w;
    int numrows;
    erow *row;
    int dirty;
    char *filename;
    char statusmsg[STATUS_MSG_LEN];
    time_t statusmsg_time;
    struct editorSyntax *syntax;
} eState;

#endif