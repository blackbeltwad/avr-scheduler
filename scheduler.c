#include "scheduler.h"
#include "timer.h"
#include <avr/common.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_SIZE 256
#define SPH (*(volatile unsigned char *)0x5E)

// Start before the first valid index; task_create() increments before use.
struct scheduler scheduler = {
    .tasks = {0}, .current_task = NULL, .task_index = -1, .total_tasks = -1};

void task_create(struct task *task, void (*entry_point)(void)) {
  // Stack grows downwards, move stack pointer to end of array
  task->stack_pointer = &task->stack[MAX_SIZE - 1];
  task->stack[0] = 0xFF;

  // Store the function address for the first context switch
  uint16_t entry_point_address = (uint16_t)entry_point;
  uint8_t entry_point_high_byte = entry_point_address >> 8;
  uint8_t entry_point_low_byte = entry_point_address & 0x00FF;

  *task->stack_pointer = entry_point_low_byte;
  // We pushed a byte so we DECREMENT
  task->stack_pointer--;
  *task->stack_pointer = entry_point_high_byte;
  task->stack_pointer -= POP_SIZE;

  scheduler.task_index++;
  scheduler.total_tasks++;
  scheduler.tasks[scheduler.task_index] = task;
  scheduler.current_task = task;
}

uint16_t store_and_pop_stack_pointer(uint16_t stack_address) {

  scheduler.current_task->stack_pointer = (uint8_t *)stack_address;

  if (scheduler.total_tasks == scheduler.task_index) {
    scheduler.task_index = 0;
  } else {
    scheduler.task_index++;
  }
  // Prepare current task for pop
  scheduler.current_task = scheduler.tasks[scheduler.task_index];
  return (uint16_t)scheduler.current_task->stack_pointer;
}

void scheduler_start() {

  volatile uint8_t *stack_pointer = scheduler.current_task->stack_pointer;
  stack_pointer += POP_SIZE;

  uint8_t function_address_high_byte = *stack_pointer;
  stack_pointer++;
  uint8_t function_address_low_byte = *stack_pointer;

  uint16_t function_address =
      ((uint16_t)function_address_high_byte << 8) | function_address_low_byte;

  void (*start_function)(void) = (void (*)(void))function_address;

  uint16_t stack_pointer_high_address = (uint16_t)stack_pointer >> 8;
  uint16_t stack_pointer_low_address = (uint16_t)stack_pointer & 0x00FF;

  SPH = (uint8_t)stack_pointer_high_address;
  SPL = (uint8_t)stack_pointer_low_address;

  start_function();
}
