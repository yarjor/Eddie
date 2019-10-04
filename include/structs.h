#ifndef STRUCTS_H
#define STRUCTS_H

#include <time.h>
#include <termios.h>

typedef struct erow {
    /** struct for holding a displayed row in the editor.
     * size and chars represent the actual content of the row,
     * and rsize and render represent the row rendered to the screen
     * (f.e - tabs, which are rendered as spaces, will appear as spaces
     * in render)
     */
    int size;
    int rsize;
    char *chars;
    char *render;
} erow;

struct editorConfig {
    int cx, cy; /** cursor location across file */
    int rx; /** rendered cursor location */
    int rowoff; /** row offset of the first displayed row into the file */
    int coloff; /** column offset of the first displayed column into the file */
    int screenrows;
    int screencols;
    int numrows;
    erow *row;
    int dirty; /** whether the file was modified since open / save */
    char *filename;
    char statusmsg[80];
    time_t statusmsg_time;
    struct termios orig_termios;
};

struct editorConfig E; /** Global editor config */

#endif