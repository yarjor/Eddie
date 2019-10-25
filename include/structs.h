#ifndef STRUCTS_H
#define STRUCTS_H

#include "syshead.h"

/**
 * @brief contains the syntax highlighting information for a certain filetype
 * 
 */
struct editorSyntax {
    char *filetype;
    char **filematch; /** patterns to match filename against */
    char **keywords;  /** first type of keywords is regular, second type is terminated with a pipe */
    char *singleline_comment_start;
    char *multiline_comment_start;
    char *multiline_comment_end;
    int flags;
};

typedef struct erow {
    /** struct for holding a displayed row in the editor.
     * size and chars represent the actual content of the row,
     * and rsize and render represent the row rendered to the screen
     * (f.e - tabs, which are rendered as spaces, will appear as spaces
     * in render)
     */
    int idx;
    int size;
    int rsize;
    int wraps;
    int *wrap_stops; /* array that states the point of wrapping in each wrap of the row */
    char *chars;
    char *render;
    unsigned char *hl; /** highlight - array of character types matching render */
    unsigned char *bg;
    int hl_open_comment;
} erow;

struct editorConfig {
    int cx, cy; /** cursor location across file */
    int rx;     /** rendered cursor location */
    int ix, iy;
    int wrapoff; /** wrap offset into current row */
    int rowoff; /** row offset of the first displayed row into the file */
    int coloff; /** column offset of the first displayed column into the file */
    int screenrows;
    int screencols;
    int editrows;
    int editcols;
    int linenum_w;
    int numrows;
    erow *row;
    int dirty; /** whether the file was modified since open / save */
    char *filename;
    char statusmsg[80];
    time_t statusmsg_time;
    struct editorSyntax *syntax;
    struct termios orig_termios;
};

struct editorConfig E; /** Global editor config */

#endif