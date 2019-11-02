#ifndef EDDIE_H
#define EDDIE_H

#include "structs.h"

/**
 * @brief Initiates an eState object on the heap without a file,
 *        (terminal state is properly initiated).
 * 
 * @return eState* (pointer to the initiated editor state object)
 */
eState *initEditor();

int main(int argc, char *argv[]);

#endif