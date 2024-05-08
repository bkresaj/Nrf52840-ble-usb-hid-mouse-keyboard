#ifndef NUS_SERVICE_H
#define NUS_SERVICE_H

void error(void);
void configure_gpio(void);
int uart_init(void);
int init_nus_ble();

#endif  // NUS_SERVICE_H