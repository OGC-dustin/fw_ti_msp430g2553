/*
 *
 */

#include <stdint.h>

void hal_init( void )
{
    WDTCTL = WDTPW | WDTHOLD;       /* stop watchdog timer */

    /* clock setup 1 MHz */
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    /* timer setup for mS tick */
    TACCR0 = 0;                     /* stop the timer */
    TACCTL0 |= CCIE;                /* enable interrupt for CCR0 */
    TACTL = TASSEL_2 + ID_0 + MC_1; /* select SMCLK, SMCLK/1, UP mode */

    TACCR0 = 1000-1;                /* set for 1 mS compare */

    /* heartbeat LED Setup on P1.6 */
    P1SEL &= ~BIT6;                 /* 0 to select general I/O function */
    P1SEL2 &= ~BIT6;                /* 0 to select general I/O function */
    P1DIR |= BIT6;                  /* set as 1 for an output */
    P1OUT &= ~BIT6;                 /* set as 0 to turn LED off */

    /* button Setup on P1.3 */
    P1SEL &= ~BIT3;                 /* 0 to select general I/O function */
    P1SEL2 &= ~BIT3;                /* 0 to select general I/O function */
    P1DIR &= ~BIT3;                 /* set as 0 for an input */
    P1REN = BIT3;                   /* set as 1 to select internal pull function */
    P1OUT = BIT3;                   /* set as 1 to set pull up to DVcc */

    /* button LED Setup on P1.0 */
    P1SEL &= ~BIT0;                 /* 0 to select general I/O function */
    P1SEL2 &= ~BIT0;                /* 0 to select general I/O function */
    P1DIR |= BIT0;                  /* set as 1 for an output */
    P1OUT &= ~BIT0;                 /* set as 0 to turn LED off */

    /* serial interface ( RX on P1.1, TX on P1.2 ) */
    P1SEL |= BIT1;                  /* 1 to select USCI */
    P1SEL2 |= BIT1;                 /* 1 to select USCI */

    P1SEL |= BIT2;                  /* 1 to select USCI */
    P1SEL2 |= BIT2;                 /* 1 to select USCI */

    UCA0CTL1 |= UCSWRST;            /* 1 = USCI module in reset */

    UCA0CTL1 |= UCSSEL_2;           /* 10 = Clock source select -> SMCLK */

    /* Baud rate values pulled from reference manual table for 9600 baud */
    UCA0BR0 = 104;                  /* baud rate control 0 */
    UCA0BR1 = 0;                    /* baud rate control 1 */
    UCA0MCTL = 0x2;                 /* modulation control */

    UCA0CTL1 &= ~UCSWRST;           /* 0 = USCI module operational */

    _enable_interrupt();

    return;
}

uint64_t tick_system = 0U;
uint64_t hal_get_tick( void )
{
    return ( tick_system );
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR( void )
{
    tick_system++;                      /* increment ms_tick timestamp */
}

void hal_led_heartbeat( int8_t state )
{
    switch ( state )
    {
        case ON:
            P1OUT |= BIT6;          /* set output high */
            break;
        case OFF:
            P1OUT &= ~BIT6;         /* set output low */
            break;
        default:
            P1OUT &= ~BIT6;         /* set output low */
    };
    return;
}

uint8_t hal_read_button( void )
{
    if ( P1IN & BIT3 )               /* read button */
    {
        P1OUT &= ~BIT0; /* Button released, LED off */
        return ( 0u );
    }
    else
    {
        P1OUT |= BIT0;  /* Button pressed, LED on */
        return ( 1u );
    }
}

int hal_uart_getchar( void )
{
    int chr = -1;

    if ( IFG2 & UCA0RXIFG )
    {
        chr = UCA0RXBUF;
    }

    return ( chr );
}

int hal_uart_putchar( int c )
{
    while ( !(IFG2 & UCA0TXIFG ) ); /* BLOCKING */

    UCA0TXBUF = ( char )c;

    return ( 0 );
}

int hal_uart_putstr( const char *str )
{
    int status = -1;

    if ( str != NULL )
    {
        status = 0;

        while ( *str != '\0' )
        {
            while ( !( IFG2 & UCA0TXIFG ) );    /* BLOCKING */

            UCA0TXBUF = *str;

            if ( *str == '\n' )
            {
                while ( !( IFG2 & UCA0TXIFG ) );    /* BLOCKING */

                UCA0TXBUF = '\r';
            }

            str++;
        }
    }

    return ( status );
}
