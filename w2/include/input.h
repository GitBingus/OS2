#ifndef INPUT_H
#define INPUT_H

#include "types.h"

char getc(void);
void readline(char *buffer, u32int size); /* using a buffer size argument in readline() 
                                            is important because it prevents the function
                                            from writing past the end of the supplied buffer
                                        */

#endif