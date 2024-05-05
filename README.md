## Overview

`netopc` is a tool designed for red team operations to enable remote execution of shellcode on compromised systems. 

It functions by listening for network connections, receiving raw machine code (shellcode), and executing the received code in a separate process.

## Prerequisites

- Linux environment
- MinGW-w64 toolchain installed for cross-compiling to Windows targets
- Standard development tools (make, gcc)

## Usage

Clean the project files:

```bash
make clean
```

### Compiling for x86

Use the following command to compile the project for x86 architecture:

```bash
make CC=i686-w64-mingw32-gcc
```

### Compiling for x64

For compiling the project to support x64 architecture, use the x64 specific MinGW-w64 compiler:

```bash
make CC=x86_64-w64-mingw32-gcc
```

Execute the compiled binary on a Windows machine or in a test environment:

```bash
./netopc.exe
```