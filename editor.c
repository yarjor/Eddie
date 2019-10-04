#include "consts.h"
#include "buffer.h"

/*** editor operations ***/

void editorInsertChar(int c)
{
    if (E.cy == E.numrows)
    {
        editorAppendRow("", 0);
    }
    editorRowInsertChar(&E.row[E.cy], E.cx, c);
    E.cx++;
}