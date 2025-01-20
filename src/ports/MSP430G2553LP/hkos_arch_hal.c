/******************************************************************************
 *
 * This file is part of HalfKOS.
 * https://github.com/alairjunior/HalfKOS
 *
 * Copyright (c) 2021 Alair Dias Junior.
 *
 * HalfKOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HalfKOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HalfKOS.  If not, see <https://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

/**************************************************************************
 *
 * HalfKOS Hardware Abstraction Layer implementation for MSP430G2553
 * The pin definitions are based on the Launchpad EXP430G2
 *
 * ************************************************************************/
#include <msp430.h>
#include <core/hkos_hal.h>
#include <core/hkos_scheduler.h>
#include <stdbool.h>
#include <stddef.h>

// General Macros
//
#define BIT(x)          (1 << x)
#define arraysize(x)    (sizeof(x) / sizeof(x[0]))

/******************************************************************************
 *  Helper function to disable the watchdog timer
 *
 *****************************************************************************/
static inline void disable_wdt( void ) {
    WDTCTL = WDTPW | WDTHOLD;
}

/******************************************************************************
 *  Helper function to initialize the DCO
 *
 *****************************************************************************/
static inline void init_dco( void ) {
    // uses the calibrated frequency of 16 MHz
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
}

/******************************************************************************
 *  Helper function to start the tick timer
 *
 *****************************************************************************/
void start_tick_timer( void ) {
    // enable timer0 A0 interrupt
    TACCTL0 |= CCIE;
}

/******************************************************************************
 *  Helper function to stop the tick timer
 *
 *****************************************************************************/
static inline void stop_tick_timer( void ) {
    // enable timer0 A0 interrupt
    TACCTL0 &= ~CCIE;
}

/******************************************************************************
 *  Helper function to initialize timer A interrupts
 *
 *****************************************************************************/
static inline void init_timerA( void ) {
    // Disable timer0 A0 interrupt
    TACCTL0 &= ~CCIE;

    // Disable timer0 A1 interrupt
    TACCTL1 &= ~CCIE;

    // Stop the timer
    TACTL = MC_0;

    // Set the counter
    // Since system is going to be configured to up and down mode,
    // it will count twice TA0CCR0 to get an interrupt. So, each
    // each unit in TA0CCR0 will be 1us
    TA0CCR0 = 1000000/HKOS_HAL_TICKS_IN_A_SECOND;

    // DCO, DCO/8, up and down mode
    // 2MHz oscillator
    TACTL = TASSEL_2 | ID_3 | MC_3;
}

/******************************************************************************
 * Before doing anything, we need to restart the stack. Stack is set to the
 * end of RAM. However, we are using the entire RAM with our structures or
 * global data. So, we need to point the stack to its structures.
 *
 * We could have done that using the linker script, but the goal here is to
 * make the system easy to understand and port.
 *
 *****************************************************************************/
static void restart_stack( void ) {

    // We now point the stack to the proper location in HalfKOS ram struct
    // not before copying the current stack so we can return from the
    // function calls
    // We are going to save from the top of the stack, to avoid overwriting.
    asm volatile (
        "mov.w      #__stack,   r15     \n\t"
        "sub.w      r1,         r15     \n\t"   // r15 has number of bytes on the stack
        "mov.w      %0,         r14     \n\t"
        "add.w      %1,         r14     \n\t"   // r14 points to the bottom of new stack
        "sub.w      r15,        r14     \n\t"   // r14 points to the top of new stack
    "loop:                              \n\t"
        "cmp        #__stack,   r1      \n\t"
        "jz         end_loop            \n\t"
        "mov.w      @r1,       @r14     \n\t"
        "incd.w     r1                  \n\t"
        "incd.w     r14                 \n\t"
        "jmp        loop                \n\t"
    "end_loop:                          \n\t"
        "sub.w      r15,        r14     \n\t"
        "mov.w      r14,         r1     \n\t"
        :
        : "i" (&hkos_ram.os_stack[0]), "i" (sizeof(hkos_ram.os_stack))
        :
    );
}

/******************************************************************************
 *  Initialize the HAL and put the system at its initial state
 *
 *****************************************************************************/
void hkos_hal_init( void ) {
    disable_wdt();
    restart_stack();
    init_dco();
    init_timerA();
    __enable_interrupt();
}

/******************************************************************************
 * Initialize the task stack.
 *
 * MSP430G2553 has 12 GP registers (r4-r15). Also, when returning from an
 * interrupt, it pops the SR (r2) and PC (r0). So, we organized the stack
 * in such a way that when returning from the interrupt, all the data is
 * already in the proper places.
 *
 * @param[in]   pp_sp       a pointer to the stack pointer indicating the
 *                          memory region of the task stack
 * @param[in]   p_pc        a pointer to the beginning of the task code
 * @param[in]   stack_size  the size of the task's stack
 *
 * @return  The value of stack pointer after the stack initialization
 *
 *****************************************************************************/
void* hkos_hal_init_stack( void* p_sp, void* p_pc, hkos_size_t stack_size ){

    uint16_t* p_stack = (uint16_t*)p_sp;


    *--p_stack = (uint16_t)p_pc;
    *--p_stack = (uint16_t)GIE;
    *--p_stack = (uint16_t)0xFFFF; // R15
    *--p_stack = (uint16_t)0xEEEE; // R14
    *--p_stack = (uint16_t)0xDDDD; // R13
    *--p_stack = (uint16_t)0xCCCC; // R12
    *--p_stack = (uint16_t)0xBBBB; // R11
    *--p_stack = (uint16_t)0xAAAA; // R10
    *--p_stack = (uint16_t)0x9999; // R9
    *--p_stack = (uint16_t)0x8888; // R8
    *--p_stack = (uint16_t)0x7777; // R7
    *--p_stack = (uint16_t)0x6666; // R6
    *--p_stack = (uint16_t)0x5555; // R5
    *--p_stack = (uint16_t)0x4444; // R4

    uint16_t* ret_stack = p_stack;

    // We paint the stack here so we can analyse stack usage.
    // to paint it, we include the context switch region too
    if ( HKOS_PAINT_TASK_STACK ) {
        stack_size += hkos_hal_get_min_stack_size();
        while ( (uint8_t*)p_stack > (uint8_t*)p_sp - stack_size ) {
            *--p_stack = HKOS_STACK_PAINT_VALUE | ( HKOS_STACK_PAINT_VALUE << 8 );
        }
    }

    // When creating the task, the stack will hold the PC + SR + GP registers
    return ret_stack;
}


/******************************************************************************
 * Get the minimal stack size
 *
 * Depending on the CPU, the minimum stack size can be different. Besides the
 * task's user operations, the stack needs to be big enough to store data
 * during the context switch
 *
 * Minimum stack size on MSP430:
 *          + 2 Bytes for PC
 *          + 2 Bytes for SR
 *          + 12*2 Bytes for GP registers
 *          + 2 Bytes for the context switch call
 *
 *          = 30 bytes
 *
 *****************************************************************************/
inline hkos_size_t hkos_hal_get_min_stack_size( void ) {
    return 30; // better define it here than at the beginning of this file.
               // less mind jumps when analysing the code.
}


/******************************************************************************
 * Jump to the operating system
 *
 * This function will point the stack pointer to the operating system's stack
 * and start the timer to do the context switch operation. The interrupt to
 * handle the context switch must do the following operations:
 *
 *      1. Save the current task's context (stored in
 *              hkos_ram.runtime_data.p_current_task)
 *      2. Call hkos_scheduler_tick_timer
 *      3. Restore the new current task's context
 *
 *****************************************************************************/
void hkos_hal_jump_to_os( void ) {
    // We won't return from this call, so we can mess with
    // the stack pointer and registers freely

    // We paint the OS stack to help debug
    if ( HKOS_PAINT_TASK_STACK ) {
        for ( hkos_size_t i = 0; i < arraysize(hkos_ram.os_stack); ++i ) {
            hkos_ram.os_stack[i] = HKOS_STACK_PAINT_VALUE;
        }
    }

    // This will prepare the system to be in idle. This is essentially an
    // idle task but it was not included as a task to save a couple of bytes
    // of memory. The way it works is the following:
    //      1. We point the SP (R1) to the memory region reserved for the OS
    //      2. We push the hkos_idle lable position to the stack. This will
    //         be our PC when there is no other task to run
    //      3. We push the SR to the stack. In this case, SR will have LPM1
    //         bits set. This will make the CPU to turn off whenever the idle
    //         task is running. Each task has its own SR, so it will not affect
    //         other tasks. Idle task also have global interrupts enabled (GIE)
    //      4. We save back the idle task pointer to be restored when there is
    //         no tasks running
    //      5. We disable the interrupts so the idle task is not interrupted
    //         after starting the tick timer
    //      6. We start the tick timer (timer0A0) to start handling the context
    //         switch
    //      7. We simulate a return from interrupt, so PC and SR are restored,
    //         so idle task goes to its code position, we enter LPM1 state and
    //         global interrupts are enabled again.
    //
    asm volatile (
        "mov.w      %0,                  r1 \n\t" // Point r1 to the os stack
        "add.w      %1,                  r1 \n\t" // move r1 to the stack's end
        "push       #hkos_idle              \n\t" // push the idle PC
        "push       %3                      \n\t" // push SR with LPM1 enabled
        "mov.w      r1,                  %2 \n\t" // save the new os SP
        "bic.w      %4,                  r2 \n\t" // Disable interrupts
        "nop                                \n\t" // Required after dis. int.
        "call       #start_tick_timer       \n\t" // starts the tick timer
        "reti                               \n\t" // RETI will update PC and SR
    "hkos_idle:                             \n\t"
        "jmp        hkos_idle               \n\t" // when idle, we do nothing
        :
        : "i" (&hkos_ram.os_stack[0]), "i" (sizeof(hkos_ram.os_stack)),
                    "m" (hkos_ram.runtime_data.p_idle_sp),
                    "i" (GIE+LPM1_bits), "i" (GIE)
        :
    );
}


/******************************************************************************
 * Enter Critical Section
 *
 * When inside a critical section, the code cannot be preempted. This can be
 * done using software only (like Lamport's bakery algorithm), but hardware
 * support is the preferable approach. So we leave it for the HAL to decide.
 *
 * OBS: this function must not be called by user code, because it may affect
 * the time counting. All use of this function MUST be restricted to HalfKOS
 * core.
 *
 * In case of MSP430, since this is a single core / single thread CPU, we just
 * need to disable interrupts
 *
 *****************************************************************************/
void hkos_hal_enter_critical_section( void ) {
    __disable_interrupt();
}

/******************************************************************************
 * Exit Critical Section
 *
 * Exit critical section must always be called after a call to enter critical
 * section.
 *
 * OBS: this function must not be called by user code, because it may affect
 * the time counting. All use of this function MUST be restricted to HalfKOS
 * core,
 *
 * In case of MSP430, since this is a single core / single thread CPU, we just
 * need to enable interrupts so scheduler is called
 *
 *****************************************************************************/
void hkos_hal_exit_critical_section( void ) {
    __enable_interrupt();
}

/******************************************************************************
 * Save context for a context switch (interrupt version)
 *
 * This function will save the context before a context switch. It must save
 * all the context information for the current task. Beware that the function
 * is declared as naked, meaning that there will be no prologue or epilogue
 * added by the compiler.
 *
 * This version is intended to be called from an interrupt.
 *
 *****************************************************************************/
__attribute__((naked))
static void save_context_from_interrupt( void ) {

    // This requires some explanation: we begin by checking if there is a
    // current task. If not, we don't need to save the context.
    // At the top of the stack we have the PC to which we will return at
    // the end of this function. However, we need to save the context in
    // the stack, but at the same time, we cannot change the register values
    // because we are saving the context. So, we do the following:
    //      1. Leave the PC at the top of the stack (TOS)
    //      2. Push R14 first, instead of R15
    //      3. Copy the PC, that is in the second to top stack position, to R14
    //      4. Copy the content of R15 to the second to top stack position
    //      5. Push the other registers from 13 to 4
    //      6. Save the stack pointer, saving the entire context
    //      7. Now, we push the return PC (stored in R14) to the TOS
    //      8. We execute a ret, that will return to the calling function
    asm volatile (
        "hkos_hal_save_context_int:         \n\t"
        "   cmp     #0,               %0    \n\t"
        "   jz      done_save_int           \n\t"
        "   push    r14                     \n\t"
        "   mov.w   2(r1),           r14    \n\t"
        "   mov.w   r15,           2(r1)    \n\t"
        "   push    r13                     \n\t"
        "   push    r12                     \n\t"
        "   push    r11                     \n\t"
        "   push    r10                     \n\t"
        "   push    r9                      \n\t"
        "   push    r8                      \n\t"
        "   push    r7                      \n\t"
        "   push    r6                      \n\t"
        "   push    r5                      \n\t"
        "   push    r4                      \n\t"
        "   mov.w   %0,               r15   \n\t"
        "   mov.w   r1,          %c1(r15)   \n\t"
        "   push    r14                     \n\t"
        "done_save_int:                     \n\t"
        "   ret                             \n\t"
            :
            :   "m" ( hkos_ram.runtime_data.p_current_task ),
                "i" ( offsetof( hkos_task_t, p_sp ) )
            :
    );
}


/******************************************************************************
 * Save context for a context switch (general version)
 *
 * This function will save the context before a context switch. It must save
 * all the context information for the current task. Beware that the function
 * is declared as naked, meaning that there will be no prologue or epilogue
 * added by the compiler.
 *
 * This version is to be called out of the interrupt. This means, it should
 * simulate an interrupt by pushing SR (R2 to the stack).
 *
 *****************************************************************************/
__attribute__((naked))
void hkos_hal_save_context( void ) {

    // This requires some explanation: we begin by checking if there is a
    // current task. If not, we don't need to save the context.
    // At the top of the stack we have the PC to which we will return at
    // the end of this function. However, we need to save the context in
    // the stack, but at the same time, we cannot change the register values
    // because we are saving the context. So, we do the following:
    //      1. Leave the PC at the top of the stack (TOS)
    //      2. Push R15 first
    //      3. Copy the PC, that is in the second to top stack position, to R15
    //      4. Copy the content of SR (R2) to the second to top stack position
    //      5. Push the other registers from 14 to 4
    //      6. Save the stack pointer, saving the entire context
    //      7. Now, we push the return PC (stored in R15) to the TOS
    //      8. We execute a ret, that will return to the calling function
    asm volatile (
        "hkos_hal_save_context:             \n\t"
        "   cmp     #0,               %0    \n\t"
        "   jz      done_save               \n\t"
        "   push    r15                     \n\t"
        "   mov.w   2(r1),           r15    \n\t"
        "   mov.w   r2,            2(r1)    \n\t"
        "   push    r14                     \n\t"
        "   push    r13                     \n\t"
        "   push    r12                     \n\t"
        "   push    r11                     \n\t"
        "   push    r10                     \n\t"
        "   push    r9                      \n\t"
        "   push    r8                      \n\t"
        "   push    r7                      \n\t"
        "   push    r6                      \n\t"
        "   push    r5                      \n\t"
        "   push    r4                      \n\t"
        "   mov.w   %0,              r14    \n\t"
        "   mov.w   r1,         %c1(r14)    \n\t"
        "   push    r15                     \n\t"
        "done_save:                         \n\t"
        "   ret                             \n\t"
            :
            :   "m" (hkos_ram.runtime_data.p_current_task),
                "i" ( offsetof( hkos_task_t, p_sp ) )
            :
    );
}

/******************************************************************************
 * Restore context after a context switch
 *
 * This function will restore the context after a context switch. It must
 * restore all the context of the new current task. Beware that the function
 * is declared as naked, meaning that there will be no prologue or epilogue
 * added by the compiler. Most probably the function will be written using
 * inline assembly and some trickies will be needed to make it work properly.
 * Check the already ported platforms for insights on how to do it.
 *
 *****************************************************************************/
__attribute__((naked))
void hkos_hal_restore_context( void ) {
    asm (
        "hkos_hal_restore_context:          \n\t"
        "   pop     r14                     \n\t" // drop the return address
        "   cmp     #0,               %0    \n\t"
        "   jz      go_idle                 \n\t"
        "   mov.w   %0,              r15    \n\t"
        "   mov.w   %c1(r15),         r1    \n\t"
        "   pop     r4                      \n\t"
        "   pop     r5                      \n\t"
        "   pop     r6                      \n\t"
        "   pop     r7                      \n\t"
        "   pop     r8                      \n\t"
        "   pop     r9                      \n\t"
        "   pop     r10                     \n\t"
        "   pop     r11                     \n\t"
        "   pop     r12                     \n\t"
        "   pop     r13                     \n\t"
        "   pop     r14                     \n\t"
        "   pop     r15                     \n\t"
        "   jmp     done_restore            \n\t"
        "go_idle:                           \n\t"
        "   mov.w   %2,               r1    \n\t"
        "done_restore:                      \n\t"
        "   reti                            \n\t"
            :
            :   "m" ( hkos_ram.runtime_data.p_current_task ),
                "i" ( offsetof( hkos_task_t, p_sp ) ),
                "m" ( hkos_ram.runtime_data.p_idle_sp )
            :
    );
}

/******************************************************************************
 * TIMER0_A0 ISR. This is the HalfKOS tick timer
 *
 * This interrupt is responsible for context switch and perform the following
 * operations:
 *
 *      1. Save the current task's context (stored in hkos_ram.current_task)
 *      2. Call hkos_scheduler_tick_timer
 *      3. Restore the new current task's context
 *
 * OBS: RETI is executed by hkos_hal_restore_context
 *
 *****************************************************************************/
__attribute__((naked))
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void timer_a0_isr(void) {
    save_context_from_interrupt();
    hkos_scheduler_tick_timer();
    hkos_hal_restore_context();
}
