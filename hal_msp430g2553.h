#ifndef HAL_MSP430G2553_H
#define HAL_MSP430G2553_H

void hal_init( void );

uint64_t hal_get_tick( void );

void hal_led_heartbeat( int8_t state ); /* for heartbeat application */

uint8_t hal_read_button( void );        /* for button monitor ( polled ) application */

int hal_uart_getchar( void );           /* for serial handler application */
int hal_uart_putchar( int c );
int hal_uart_putstr( const char *str );

#endif /* HAL_MSP430G2553_H */
