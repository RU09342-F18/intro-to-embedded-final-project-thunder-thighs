#include <intrinsics.h>
#include <stdint.h>
#include <msp430.h>

#define TRIGGER_PIN BIT1   // P6.1
#define ECHO_PIN BIT3  // P1.3
#define Danger 10  // cm
#define Caution 25 // cm
#define Alert 36  // cm
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
    WDTCTL = WDTPW | WDTHOLD;   //disable watchdog timer

    // Configure trigger pin, low to start
    P6DIR |= TRIGGER_PIN;
    P6OUT &= ~TRIGGER_PIN;

    // Configure LEDs, off to start
    P1DIR |= BIT2;      //Red
    P1OUT &= ~BIT2;     //Red
    P1DIR |= BIT4;      //Yellow
    P1OUT &= ~BIT4;     //Yellow
    P1DIR |= BIT5;      //Blue
    P1OUT &= ~BIT5;     //Blue
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

    int16_t lastCount = 0;
    int32_t distance = 0;
    
    //UART setup
    P4SEL |= BIT4 | BIT5;                                       // Pin4.4 set as TXD output,  Pin4.5 set as RXD input
    UCA1CTL1 |= UCSWRST;                                        // State Machine Reset + Small Clock Initialization
    UCA1CTL1 |= UCSSEL_1;                                       // Sets USCI Clock Source to SMCLK (32kHz)
    UCA1BR0 = 0x03;                                             // Setting the Baud Rate to be 9600
    UCA1BR1 = 0x00;                                             // Setting the Baud Rate to be 9600
    UCA1MCTL = UCBRS_3+UCBRF_0;                                 // Modulation UCBRSx=3, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;                                       // Initialize USCI State Machine
    UCA1IE |= UCRXIE;

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

        if (distance <= Danger)
        {
            // Turn on LED Red
            P1OUT |= BIT2;
        }
        if (distance <= Caution)
        {
            //Turn on LED Yellow
            P1OUT |= BIT4;
        }
        if (distance <= Alert)
        {
            //Turn on LED Blue
            P1OUT |= BIT5 ;
        }
        else
        {
                   // Turn off LEDs
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
