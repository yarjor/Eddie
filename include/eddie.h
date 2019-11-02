#ifndef EDDIE_H
#define EDDIE_H

#include "structs.h"

/**
 * @brief Initiates an eState object on the heap without a file,
 *        (terminal state is properly initiated).
 *        Returns a pointer to the object.
 * 
 * @return eState* 
 */
eState *initEditor();

int main(int argc, char *argv[]);

#endif