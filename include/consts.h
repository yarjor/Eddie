#ifndef CONSTS_H
#define CONSTS_H

#include <stdio.h>
#include <stdarg.h>

#include "structs.h"

#ifndef DEBUG_PRINTS
#define DEBUG_PRINTS 0
#endif /* DEBUG */

/*** Utils ***/
#define debug_printf(fmt, ...) \
            do { if (DEBUG_PRINTS) fprintf(stderr, "%s:%d:%s() - " fmt "\n", __FILE__, \
                                    __LINE__, __func__, __VA_ARGS__); } while (0)

#define debug_print(s) debug_printf("%s", s)

/*** Application consts ***/

#define EDDIE_VERSION "0.0.3"

/*** Editor config ***/

#define EDDIE_TAB_STOP 4
#define MSG_TIMEOUT 5 // seconds
#define EDDIE_QUIT_TIMES 3
#define DO_SOFTWRAP // TODO: Choose by configuration

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

#define ESCAPE '\x1b'
#define ANSI_CLEAR_SCREEN "\x1b[2J"
#define ANSI_HOME_CURSOR "\x1b[H"
#define ANSI_GET_CURSOR_POSITION "\x1b[6n"
#define ANSI_CURSOR_FORWARD(n) "\x1b" #n "C"
#define ANSI_CURSOR_DOWN(n) "\x1b[" #n "B"
#define ANSI_ERASE_TO_RIGHT "\x1b[K"
#define ANSI_REVERSE_VIDEO "\x1b[7m"
#define ANSI_CLEAR_ATTR "\x1b[m"
#define ANSI_HIDE_CURSOR "\x1b[?25l"
#define ANSI_SHOW_CURSOR "\x1b[?25h"
#define ANSI_CURSOR_POS_FMT "\x1b[%d;%dH"

/* ANSI styles */
#define _ANSI_STYLE(n) "\x1b[" #n "m"
#define ANSI_STYLE(n) _ANSI_STYLE(n)
#define ANSI_STYLE_FMT "\x1b[%dm"
#define STYLE_UNDERLINE 4
#define STYLE_NO_UNDERLINE 24

#define STYLE_BLACK_FG 30
#define STYLE_RED_FG 31
#define STYLE_GREEN_FG 32
#define STYLE_YELLOW_FG 33
#define STYLE_BLUE_FG 34
#define STYLE_MAGENTA_FG 35
#define STYLE_CYAN_FG 36
#define STYLE_WHITE_FG 37
#define STYLE_DEFAULT_FG 39

#define STYLE_BRIGHT_BLACK_FG 90
#define STYLE_BRIGHT_RED_FG 91
#define STYLE_BRIGHT_GREEN_FG 92
#define STYLE_BRIGHT_YELLOW_FG 93
#define STYLE_BRIGHT_BLUE_FG 94
#define STYLE_BRIGHT_MAGENTA_FG 95
#define STYLE_BRIGHT_CYAN_FG 96
#define STYLE_BRIGHT_WHITE_FG 97

#define STYLE_BLACK_BG 40
#define STYLE_RED_BG 41
#define STYLE_GREEN_BG 42
#define STYLE_YELLOW_BG 43
#define STYLE_BLUE_BG 44
#define STYLE_MAGENTA_BG 45
#define STYLE_CYAN_BG 46
#define STYLE_WHITE_BG 47
#define STYLE_DEFAULT_BG 49

#define STYLE_BRIGHT_BLACK_BG 100
#define STYLE_BRIGHT_RED_BG 101
#define STYLE_BRIGHT_GREEN_BG 102
#define STYLE_BRIGHT_YELLOW_BG 103
#define STYLE_BRIGHT_BLUE_BG 104
#define STYLE_BRIGHT_MAGENTA_BG 105
#define STYLE_BRIGHT_CYAN_BG 106
#define STYLE_BRIGHT_WHITE_BG 107

#define ANSI_STYLE_DEFAULT_BOTH  \
    ANSI_STYLE(STYLE_DEFAULT_FG) \
    ANSI_STYLE(STYLE_DEFAULT_BG)

/* Specific Styles */
#define LINENUM_STYLE_ON ANSI_STYLE(STYLE_BRIGHT_BLACK_BG)
#define LINENUM_STYLE_OFF ANSI_STYLE(STYLE_DEFAULT_BG)

#endif