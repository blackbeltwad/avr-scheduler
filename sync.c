#include "sync.h"
#include <stddef.h>

void lock_mutex(mutex_t *mutex) {
  if (mutex->owner == NULL) {
    mutex->owner = get_current_task();
  }
}
void unlock_mutex(mutex_t *mutex) {
  if (get_current_task() == mutex->owner) {
    mutex->owner = NULL;
  }
}
