#include <msp430.h> 


/**
 * main.c
 */
float time, adjustedTime;
void initializeGPIO () {
    P1DIR |= BIT2;      //pin 1.2 trigger
    P6DIR |= BIT6;      //pin 6.0 echo
}

void initializeTimerSend() {
    TA0CTL = TASSEL_2 + MC_1 + ID_0 + TACLR;
    TA0CCR0 = 0;
    TA0CCTL1 |= OUTMOD_3;
}

void initializeTimerT() {
    TA1CTL = TASSEL_1 + MC_1 + ID_0 + TACLR;
    TA1CCR0 = 0;
    TA1CCTL1 |= OUTMOD_3;
}

void initializeUART() {
    P4SEL |= BIT4 + BIT5;
    P3SEL |= BIT3 + BIT4;

    UCA1CTL1 |= UCSWRST;                    //reset state machine
    UCA1CTL1 |= UCSSEL_2;                   //SMCLK for UART
    UCA1BR0 = 104;                          //
    UCA1BR1 = 0;                            //
    UCA1MCTL |= UCBRS_1 + UCBRF_0;          //Modulation
    UCA1CTL1 &= ~UCSWRST;                   //Initialize USCI state machine
    UCA1IE |= UCRXIE;                       //Enable USCI_A0 RX interrupt
    UCA1IFG &= ~UCRXIFG;                                        //Reset/Clear interrupt flags*/
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	void initializeGPIO();
	void initializeTimer();
	void initializeUART();
	
	return 0;
}

#pragma vector = PORT6_VECTOR
__interrupt void Port_6(void){
    TA1CCTL = MC_0;         //stop timer
    time = TA1R;            //get time value
    distance = time/58;     //convert to distance
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_Send (void) {
    P1DIR ^= BIT2;          //send trigger
    TA1CCTL = MC_1;         //start timer
}
