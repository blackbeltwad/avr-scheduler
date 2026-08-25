# AVR Scheduler

A small bare-metal task scheduler for the **ATmega328P**, written in C and AVR assembly.

The goal of this project is to understand how operating-system-style task scheduling works at the lowest level by implementing the core mechanisms directly on the microcontroller without relying on an existing RTOS.

## What It Does

The scheduler allows multiple tasks to run on the ATmega328P by periodically switching between their CPU contexts.

Each task has its own stack and saved CPU state. A timer interrupt triggers the context switch:

```text
Task A
  ↓
Timer Interrupt
  ↓
Save CPU Context
  ↓
Save Task A Stack Pointer
  ↓
Load Task B Stack Pointer
  ↓
Restore Task B Context
  ↓
RETI
  ↓
Task B
```

When Task A is selected again, its previously saved stack pointer and registers are restored, allowing it to continue execution from where it was interrupted.

## Current Features

* Task creation
* Independent stack for each task
* Timer-driven task switching
* CPU register context saving/restoring
* Stack pointer switching
* Initial task context creation
* Cooperative task state through persistent execution contexts
* AVR assembly context-switch implementation
* Bare-metal ATmega328P operation

## Example Tasks

The project currently demonstrates two simple tasks:

```c
void FuncA(void) {
    DDRD |= (1 << 7);
    PORTD |= (1 << 7);

    while (1) {
    }
}

void FuncB(void) {
    DDRD &= ~(1 << 7);
    PORTD &= ~(1 << 7);

    while (1) {
    }
}
```

`FuncA` turns an output on, while `FuncB` turns it off. The scheduler switches between the tasks using a timer interrupt.

The important part is that switching tasks does **not** call the task function again. Once a task has started, its CPU context is saved when it is interrupted and restored when the task runs again.

## Architecture

The project targets:

* **MCU:** ATmega328P
* **Architecture:** 8-bit AVR
* **Clock:** 16 MHz
* **Language:** C
* **Assembly:** AVR Assembly
* **Compiler:** `avr-gcc`

The scheduler is split between C and assembly:

```text
C
├── Task management
├── Task creation
├── Scheduler logic
└── Timer configuration

Assembly
└── CPU context switching
    ├── Save registers
    ├── Save stack pointer
    ├── Switch stack
    ├── Restore registers
    └── Return from interrupt
```

## Task Context

A task's context consists primarily of its CPU execution state and stack.

Conceptually:

```text
Task A
┌─────────────────────┐
│ CPU registers       │
│ Status register     │
│ Stack pointer       │
│ Task stack          │
└─────────────────────┘

Task B
┌─────────────────────┐
│ CPU registers       │
│ Status register     │
│ Stack pointer       │
│ Task stack          │
└─────────────────────┘
```

When the scheduler switches tasks, it saves the current context and restores the next task's context.

Hardware peripherals such as `PORTD`, `DDRD`, timers, UART registers, etc. are shared MCU hardware and are not part of an individual task's private stack.

## Project Structure

```text
avr-scheduler/
├── main.c
├── scheduler.c
├── scheduler.h
├── timer.c
├── timer.h
├── context.S
└── Makefile
```

## Building

Build the project with:

```bash
make
```

This produces the scheduler ELF executable.

A HEX file can then be generated with:

```bash
avr-objcopy -O ihex -R .eeprom scheduler.elf scheduler.hex
```

## Debugging

The project is developed with debugging in mind and can be inspected using AVR GDB.

Useful things to inspect include:

* `$sp` — current stack pointer
* `$pc` — current program counter
* CPU registers
* Saved task stack pointers
* Task stack contents
* Context-switch instructions
* Timer interrupt execution

The scheduler can therefore be followed directly at both the C and assembly levels.

## Future Work

The core task-switching mechanism is currently implemented. Planned scheduler functionality includes:

### Priority Scheduling

Tasks will have priorities, allowing the scheduler to select the highest-priority runnable task.

```text
High Priority Task
        ↓
Medium Priority Task
        ↓
Low Priority Task
```

### Mutexes

Mutexes will be added to protect shared resources from simultaneous access by multiple tasks.

```text
Task A ──┐
         ├── Mutex ── Shared Resource
Task B ──┘
```

Only one task will be able to own a mutex at a time.

### Semaphores

Semaphores will provide task synchronization and signaling mechanisms.

They will allow one task to signal another task or control access to a limited number of resources.

## Purpose

This project is primarily a learning exercise in:

* Operating-system fundamentals
* RTOS architecture
* CPU context switching
* Interrupts
* Stack management
* AVR assembly
* Calling conventions
* Task scheduling
* Synchronization primitives
* Bare-metal embedded programming

The long-term goal is to build the scheduler into a small RTOS-like system while understanding each component rather than relying on an existing RTOS implementation.

