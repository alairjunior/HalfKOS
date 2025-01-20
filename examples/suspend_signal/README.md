# HalfKOS Suspend/Signal Example

In this example, two tasks are used to show how to suspend a task and how to signal
it to resume.

Toolchain used for this example:

1. [msp430-gcc](https://www.ti.com/tool/MSP430-GCC-OPENSOURCE)
2. [mspdebug](https://dlbeer.co.nz/mspdebug/)


Makefile targets:

1. **all**: build the ELF without any special flags
2. **debug**: build the ELF with debug flags
3. **release**: build the ELF with O3 optimization
4. **disassemble**: generate the dump of the generated ELF
5. **run**: start mspdebug and program the ELF to the target
6. **clean**: clear the build

