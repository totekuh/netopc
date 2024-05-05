#ifndef EXEC_H
#define EXEC_H

#include <windows.h>

void execute_shellcode(BYTE *shellcode, size_t shellcode_size, int interactive);

#endif
