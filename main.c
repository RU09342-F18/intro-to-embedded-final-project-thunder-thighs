#include <msp430.h> 


/**
 * main.c
 */
float time, distance;

void initializeGPIO () {
    P1DIR |= BIT0;      //led test pin
    P1DIR |= BIT3;      //pin 1.3 echo
    P2DIR |= BIT0;      //pin 2.0 trigger
    P1IFG &= ~BIT3;     //clear interrupt flag
    P1IE |= BIT3;       //enable port interrupt
    P1IES &= ~BIT3;     //set interrupt edge low/high
}

void initializeTimerSend() {
    TA0CTL = TASSEL_2 + MC_1 + TACLR;       //SMCLK in up mode
    TA0CCR0 = 20000;           //send every 20000us
    TA0CCTL0 = CCIE;        //enable interrupt
}

void initializeTimerT() {
    TA1CTL = TASSEL_2 + MC_0  + TACLR;      //SMCLK in stop mode
    TA1CCR0 = 50000;
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
	initializeGPIO();
	initializeTimerSend();
	initializeUART();
	initializeTimerT();
	__bis_SR_register(GIE);
	while(1);
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){

    TA1CTL = MC_0;          //stop timing timer
    time = TA1R;            //get time value
    distance = time/58;     //convert to distance
    TA1CTL = TACLR;         //clear timing timer
    TA0CTL = MC_1;          //start send timer
    P1IFG &= ~BIT3;         //clear interrupt flag
    P1OUT &= ~BIT3;         //set trigger low

}

#pragma vector = TIMER0_A0_VECTOR           //send timer
__interrupt void Timer_Send (void){

    P2OUT |= BIT0;          //send trigger
    TA0CTL = MC_0;          //stop send timer
    TA0CTL = TACLR;         //reset send timer
    TA0IV = 0;              //clear send timer interrupt flag
    TA1CTL = MC_2;          //start timing timer in continuous mode

}

