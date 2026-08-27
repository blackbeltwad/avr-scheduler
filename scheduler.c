#include "scheduler.h"
#include <avr/common.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#define SPH (*(volatile unsigned char *)0x5E)
#define POP_SIZE 34
#define SPH (*(volatile unsigned char *)0x5E)

struct scheduler {
  struct task *tasks[MAX_STACKS];
  struct task *current_task;
  int8_t task_index;
  int8_t total_tasks;
};

// Private methods
uint16_t store_and_pop_stack_pointer(uint16_t stack_address);
void update_sleep_timer(void);
struct task *select_next_task(void);

// Start before the first valid index; task_create() increments before use.
struct scheduler scheduler = {
    .tasks = {0}, .current_task = NULL, .task_index = -1, .total_tasks = -1};

void task_create(struct task *task, void (*entry_point)(void),
                 uint8_t priority_value) {
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
  scheduler.current_task->priority = priority_value;
}

uint16_t store_and_pop_stack_pointer(uint16_t stack_address) {

  scheduler.current_task->stack_pointer = (uint8_t *)stack_address;
  /*
    if (scheduler.total_tasks == scheduler.task_index) {
      scheduler.task_index = 0;
    } else {
      scheduler.task_index++;
    }
  */
  // Prepare current task for pop
  scheduler.current_task = select_next_task();
  return (uint16_t)scheduler.current_task->stack_pointer;
}

struct task *select_next_task() {

  struct task *highest_priority_task;
  uint8_t highest_priority = 0;

  for (int i = 0; i < scheduler.total_tasks; i++) {
    struct task *this_task = scheduler.tasks[i];

    if (this_task->state == TASK_READY) {
      if (this_task->priority > highest_priority) {
        highest_priority = this_task->priority;
        highest_priority_task = this_task;
      }
    }
  }
  highest_priority_task->state = TASK_RUNNING;
  return highest_priority_task;
}
// Get the function address at the start of the tasks stack and call it
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

  // We set stack pointer here to save the two bytes we used in task creation
  SPH = (uint8_t)stack_pointer_high_address;
  SPL = (uint8_t)stack_pointer_low_address;

  start_function();
}

void update_sleep_timer() {

  for (uint8_t i = 0; i < scheduler.total_tasks; i++) {
    struct task *this_task = scheduler.tasks[i];

    if (this_task->state == TASK_SLEEPING) {
      this_task->sleep_remaining -= 1;

      if (this_task->sleep_remaining <= 0) {
        this_task->state = TASK_READY;
      }
    }
  }
}

void task_yield() {
  // Force context switch
  OCR1AL = 0;
  OCR1AL = 0;
  TIFR1 |= (1 << 1);
}

void task_block() {
  scheduler.current_task->state = TASK_BLOCKED;
  task_yield();
}

void task_sleep(double time_ms) {
  scheduler.current_task->state = TASK_SLEEPING;
  scheduler.current_task->state = time_ms;
  task_yield();
}
