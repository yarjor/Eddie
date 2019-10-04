#ifndef CONSTS_H
#define CONSTS_H

#include "structs.h"

// Application consts

#define KILO_VERSION "0.0.1"

// Editor config

#define KILO_TAB_STOP 8

// Editor keys

#define CTRL_KEY(k) ((k)&0x1f)

    enum editorKey {
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

#endif // CONSTS_H