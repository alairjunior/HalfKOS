# HalfKOS Blink Example for MSP430G2553 Launchpad

This is a very simple example on how to configure HalfKOS for running basic tasks.
In this example, there are two tasks: one for the Green LED and the other for the
Red LED. If everything goes right, the LEDs should blink more or less in sync, but
not at the same time possibly with some overlapping. If the LEDs blink in sync and
always at the same time, most probably there is an error and blink_error was invoked.

In this example, both tasks are created before starting HalfKOS and they execute
while the system is powered up. Since there is no idle time, the system never goes
into low power mode.

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

