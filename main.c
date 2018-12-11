#include <intrinsics.h>
#include <stdint.h>
#include <msp430.h>

#define TRIGGER_PIN BIT1   // P6.1
#define ECHO_PIN BIT3  // P1.3
#define LED_PIN BIT0   // P1.0
#define DISTANCE_THRESHOLD 30  // cm
#define DISTANCE_THRESHOLD2 75  // cm
#define DISTANCE_THRESHOLD3 115  // cm
#define MEASURE_INTERVAL 2048  // ~250 ms

void triggerMeasurement() {
    // Start trigger
    P6OUT |= TRIGGER_PIN;

    // Wait a small amount of time with trigger high, > 10us required (~10 clock cycles at 1MHz MCLK)
    __delay_cycles(10);

    // End trigger
    P6OUT &= ~TRIGGER_PIN;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    // Configure trigger pin, low to start
    P6DIR |= TRIGGER_PIN;
    P6OUT &= ~TRIGGER_PIN;

    // Configure LED, off to start
    P1DIR |= BIT2;
    P1OUT &= ~BIT2;
    P1DIR |= BIT4;
    P1OUT &= ~BIT4;
    P1DIR |= BIT5;
    P1OUT &= ~BIT5;
    // Configure echo pin as capture input to TA0CCR2
    P1DIR &= ~ECHO_PIN;
    P1SEL |= ECHO_PIN;

    // Set up TA0 to capture in CCR2 on both edges from P1.3 (echo pin)
    TA0CCTL2 = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    // Set up TA0 to compare CCR0 (measure interval)
    TA0CCR0 = MEASURE_INTERVAL;
    TA0CCTL0 = CCIE;

    // Set up TA0 with ACLK / 4 = 8192 Hz
    TA0CTL = TASSEL__ACLK | ID__4 | MC__CONTINUOUS | TACLR;

    uint16_t lastCount = 0;
    uint32_t distance = 0;
//UART setup
    P4SEL |= BIT4;                            // UART TX
      P4SEL |= BIT5;                            // UART RX

      UCA1CTL1 |= UCSWRST;                      // Clears the UART control register 1
      UCA1CTL1 |= UCSSEL_2;                     // Sets SMCLK
      UCA1BR0 = 104;                            // For baud rate of 9600
      UCA1BR1 = 0;                              // For baud rate of 9600

      UCA1MCTL |= UCBRS_2;                      // set modulation pattern to high on bit 1 & 5
      UCA1CTL1 &= ~UCSWRST;                     // initialize USCI
      UCA1IE |= UCRXIE;                         // enable USCI_A1 RX interrupt
      UCA1IFG &= ~UCRXIFG;                      // clears interrupt flags

    for (;;)
    {
        triggerMeasurement();

        // Wait for echo start
        __low_power_mode_3();

        lastCount = TA0CCR2;

        // Wait for echo end
        __low_power_mode_3();

        distance = TA0CCR2 - lastCount;
        distance *= 34000;
        distance >>= 14;  // division by 16384 (2 ^ 14)

        if (distance <= DISTANCE_THRESHOLD)
        {
            // Turn on LED
            P1OUT |= BIT2;
        }
        if (distance <= (DISTANCE_THRESHOLD2))
        {
            // Turn off LED
            P1OUT |= BIT4;
        }
        if (distance <= (DISTANCE_THRESHOLD3))
                {
                    // Turn off LED
                    P1OUT |= BIT5 ;
                }
        else
        {
                   // Turn off LED
                   P1OUT &= ~BIT2;
                   P1OUT &= ~BIT4;
                   P1OUT &= ~BIT5;
        }
        UCA1TXBUF = distance;

        // Wait for the next measure interval tick
        __low_power_mode_3();
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void) {
    // Measure interval tick
    __low_power_mode_off_on_exit();
    TA0CCR0 += MEASURE_INTERVAL;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void) {
    // Echo pin state toggled
    __low_power_mode_off_on_exit();
    TA0IV = 0;
}
