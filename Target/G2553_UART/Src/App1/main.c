#include "msp430.h"
#include <stdint.h>
#include "TI_MSPBoot_Mgr_Vectors.h"

/******************************************************************************
 *
 * @brief   Main function
 *  This example application performs the following functions:
 *  - Toggle LED1 (P1.0) at startup (to indicate App1 execution)
 *  - Toggles LED1 using a timer periodic interrupt (demonstrates vector redirection)
 *  - Forces Boot mode when button S2 (P1.3) is pressed (demonstrates vector
 *      redirection and Boot Mode call
 *
 * @return  none
 *****************************************************************************/
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

    //------------------------------
    P1DIR |= BIT7;
    for(int i = 0; i < 5; i++){
      __delay_cycles(500000);
      P1OUT ^= BIT7;
    }
    P1OUT &= ~BIT7;
//-----------------------------------------

   

    // Start Timer interrupt
    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    CCR0 = 1000-1;
    TACTL = TASSEL_1 + MC_1;                  // ACLK, upmode

    __bis_SR_register(LPM3_bits + GIE);


  return 0;
}

/******************************************************************************
 *
 * @brief   Timer A Interrupt service routine
 *  This routine simply toggles an LED but it shows how to declare interrupts
 *   in Application space
 *   Note that this function prototype should be accessible by 
 *   TI_MSPBoot_Mgr_Vectors.c and it should be declared in ProxyVectorTable
 *
 * @return  none
 *****************************************************************************/
__interrupt void Timer_A (void)
{
  P1OUT ^= BIT7;                            // Toggle P1.0
  CCTL0 &= ~CCIFG;
}

/******************************************************************************
 *
 * @brief   Port 1 Interrupt service routine
 *  Forces Boot mode when button S2 (P1.3) is pressed
 *   Note that this function prototype should be accessible by TI_MSPBoot_Mgr_Vectors.c
 *   and it should be declared in ProxyVectorTable
 *
 * @return  none
 *****************************************************************************/
//__interrupt void P1_Isr(void)
//{
//    P1IFG = 0;
//    TI_MSPBoot_JumpToBoot();
//}

/******************************************************************************
 *
 * @brief   Dummy Interrupt service routine
 *  This ISR will be executed if an undeclared interrupt is called
 *
 * @return  none
 *****************************************************************************/
__interrupt void Dummy_Isr(void)
{
    while(1)
        ;
}

