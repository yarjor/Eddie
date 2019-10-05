#ifndef CONSTS_H
#define CONSTS_H

#include "structs.h"

#define STR_(X) #X
#define STR(X) STR_(X)

/*** Application consts ***/

#define KILO_VERSION "0.0.1"

/*** Editor config ***/

#define KILO_TAB_STOP 8
#define MSG_TIMEOUT 5 // seconds
#define KILO_QUIT_TIMES 3

/*** Keyboard ***/

#define CTRL_KEY(k) ((k)&0x1f)

enum editorKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

/*** ANSI Codes ***/

#define ESCAPE "\x1b"
#define ANSI_CLEAR_SCREEN "\x1b[2J"
#define ANSI_HOME_CURSOR "\x1b[H"
#define ANSI_GET_CURSOR_POSITION "\x1b[6n"
#define ANSI_CURSOR_FORWARD(n) "\x1b[##n##C"
#define ANSI_CURSOR_DOWN(n) "\x1b[##n##B"
#define ANSI_ERASE_TO_RIGHT "\x1b[K"
#define ANSI_REVERSE_VIDEO "\x1b[7m"
#define ANSI_CLEAR_ATTR "\x1b[m"
#define ANSI_HIDE_CURSOR "\x1b[?25l"
#define ANSI_SHOW_CURSOR "\x1b[?25h"
#define ANSI_CURSOR_POS_FMT "\x1b[%d;%dH"
#define CLEAR_SCREEN "\x1b[2J"
#define CLEAR_SCREEN "\x1b[2J"
#define CLEAR_SCREEN "\x1b[2J"
#define CLEAR_SCREEN "\x1b[2J"

#endif