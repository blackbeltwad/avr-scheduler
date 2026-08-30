#include "scheduler.h"
#include <avr/common.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#define SPH (*(volatile unsigned char *)0x5E)
#define POP_SIZE 34
#define ARG_SIZE 25
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

void task_create(struct task *task, void (*entry_point)(void *),
                 uint8_t priority_value, void *entry_argument) {
  // Stack grows downwards, move stack pointer to end of array
  task->stack_pointer = &task->stack[MAX_SIZE - 1];
  task->stack[0] = 0xFF;

  // Store the function address for the first context switch
  uint16_t entry_point_address = (uint16_t)entry_point;
  uint8_t entry_point_high_byte = entry_point_address >> 8;
  uint8_t entry_point_low_byte = entry_point_address & 0x00FF;

  // Store arg to pass into context switch R25-R24
  uint16_t entry_arg_address = (uint16_t)entry_argument;
  uint8_t entry_arg_high_byte = entry_arg_address >> 8;
  uint8_t entry_arg_low_byte = entry_arg_address & 0x00FF;

  *task->stack_pointer = entry_point_low_byte;
  // We pushed a byte so we DECREMENT
  task->stack_pointer--;
  *task->stack_pointer = entry_point_high_byte;

  task->stack_pointer -= ARG_SIZE;
  // Inside R24
  *task->stack_pointer = entry_arg_low_byte;
  // Inside R25
  task->stack_pointer--;
  *task->stack_pointer = entry_arg_high_byte;
  task->stack_pointer += ARG_SIZE + 1;
  task->stack_pointer -= POP_SIZE;

  scheduler.task_index++;
  scheduler.total_tasks++;
  scheduler.tasks[scheduler.task_index] = task;
  scheduler.current_task = task;
  scheduler.current_task->task_arg = entry_argument;
  scheduler.current_task->priority = priority_value;
  task->state = TASK_READY;
}

uint16_t store_and_pop_stack_pointer(uint16_t stack_address) {

  scheduler.current_task->stack_pointer = (uint8_t *)stack_address;
  /*
  if (scheduler.total_tasks == scheduler.task_index) {
    scheduler.task_index = 0;
  } else {
    scheduler.task_index++;
  }

  //  Prepare current task for pop
  scheduler.current_task = scheduler.tasks[scheduler.task_index];
  */
  scheduler.current_task = select_next_task();
  return (uint16_t)scheduler.current_task->stack_pointer;
}
// Get the function address at the start of the tasks stack and call it
struct task *select_next_task() {
  struct task *highest_priority_task = NULL;
  uint8_t highest_priority = 0;
  uint8_t task_count = scheduler.total_tasks + 1;

  // Find highest priority among READY tasks
  for (int i = 0; i <= scheduler.total_tasks; i++) {
    struct task *this_task = scheduler.tasks[i];

    if (this_task->state == TASK_READY &&
        this_task->priority >= highest_priority) {

      highest_priority = this_task->priority;
      highest_priority_task = this_task;
    }
  }

  // No READY task at all
  // Add a fallback here maybe
  if (highest_priority_task == NULL) {
    return scheduler.current_task;
  }

  // Search circularly for the next READY task at that priority,
  // starting just after the currently running task
  for (int i = 1; i <= scheduler.total_tasks; i++) {
    int index = (scheduler.task_index + i) % task_count;
    struct task *this_task = scheduler.tasks[index];

    if (this_task->state == TASK_READY &&
        this_task->priority == highest_priority) {

      scheduler.task_index = index;
      highest_priority_task = this_task;
      break;
    }
  }

  scheduler.current_task->state = TASK_READY;
  highest_priority_task->state = TASK_RUNNING;

  return highest_priority_task;
}
void scheduler_start() {

  volatile uint8_t *stack_pointer = scheduler.current_task->stack_pointer;
  stack_pointer += POP_SIZE;

  uint8_t function_address_high_byte = *stack_pointer;
  stack_pointer++;
  uint8_t function_address_low_byte = *stack_pointer;

  uint16_t function_address =
      ((uint16_t)function_address_high_byte << 8) | function_address_low_byte;

  void (*start_function)(volatile void *) =
      (void (*)(volatile void *))function_address;

  uint16_t stack_pointer_high_address = (uint16_t)stack_pointer >> 8;
  uint16_t stack_pointer_low_address = (uint16_t)stack_pointer & 0x00FF;

  // We set stack pointer here to save the two bytes we used in task creation
  SPH = (uint8_t)stack_pointer_high_address;
  SPL = (uint8_t)stack_pointer_low_address;

  start_function(scheduler.current_task->task_arg);
}

void update_sleep_timer() {

  for (uint8_t i = 0; i < scheduler.total_tasks; i++) {
    struct task *this_task = scheduler.tasks[i];

    if (this_task->state == TASK_SLEEPING) {
      this_task->sleep_remaining -= 1;

      if (this_task->sleep_remaining <= 0) {
        this_task->state = TASK_READY;
        this_task->sleep_remaining = 0;
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

void task_block(void *arg) {
  cli();
  struct task *this_task = arg;
  this_task->state = TASK_BLOCKED;
  task_yield();
  sei();
}

void task_unblock(void *arg) {
  cli();
  struct task *this_task = arg;
  this_task->state = TASK_READY;
  sei();
}

void task_sleep(double time_ms) {
  cli();
  scheduler.current_task->state = TASK_SLEEPING;
  scheduler.current_task->sleep_remaining = time_ms;
  task_yield();
  sei();
}

void set_priority(uint8_t priority_value) {
  scheduler.current_task->priority = priority_value;
}

struct task *get_current_task() { return scheduler.current_task; }
