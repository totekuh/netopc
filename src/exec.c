#include "exec.h"
#include <stdio.h>
#include "exec_new_process.c"

void execute_shellcode(BYTE *shellcode, size_t shellcode_size, int interactive) {
    if (interactive) {
        printf("[!] Press Enter to execute shellcode\n");
        getchar();
    }

    execute_in_new_process(shellcode, shellcode_size);
}
