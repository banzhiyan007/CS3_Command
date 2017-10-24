#ifndef HAL_TIMEOUT_H
#define HAL_TIMEOUT_H

void hal_timeout_init(void (*isr)(void));
void hal_timeout_start(int us);

#endif

