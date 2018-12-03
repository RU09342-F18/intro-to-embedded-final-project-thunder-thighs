/*Authors: Nick Scamardi, Eric schroeder, Nick Setaro
 * Written: December 3, 2018
 * Last Updated: December 3, 2018 */

#include <msp430.h> 


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	
	
	return 0;
}
