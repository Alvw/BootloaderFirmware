#include "msp430.h"
#include <stdint.h>
#include "TI_MSPBoot_Mgr_Vectors.h"

uint8_t  goToBootCounter =0;
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

    //------------------------------
    P1DIR |= BIT7;
    for(int i = 0; i < 4; i++){
      __delay_cycles(500000);
      P1OUT ^= BIT7;
    }
    P1OUT &= ~BIT7;
//-----------------------------------------

     // CLOCK
  BCSCTL1 = CALBC1_16MHZ;                    
  DCOCTL = CALDCO_16MHZ;
  BCSCTL2 |= DIVS_3;                        // SMCLK / 8;
  
  //SMCLC output pin
  P1DIR |= BIT4; //P1.4 = output direction
  P1SEL |= BIT4; //P1.4 = SMCLK output function
  
  BCSCTL1 |= XTS;                           // ACLK = LFXT1 = HF XTAL
  BCSCTL3 |= LFXT1S1;                       // 3 – 16MHz crystal or resonator
  volatile unsigned int i;
  do {
    IFG1 &= ~OFIFG;                         // Clear OSCFault flag
    for (i = 0xFF; i > 0; i--);             // Time for flag to set
  } while (IFG1 & OFIFG);                     // OSCFault flag still set?
  BCSCTL2 |= SELS + DIVS_3;                        // SMCLK = LFXT1 / 8; 

   // Timer
  TACTL |= TACLR;
  TACTL = TASSEL_2;     // SMCLK
  TACTL |= ID_2 + ID_1; // 1:8  
  TACCR0 = 0xffff;
  TACTL |= TAIE;
  TACTL &= ~TAIFG;      
  TACTL |= MC_1;

    __bis_SR_register(LPM1_bits + GIE);


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
//#pragma vector = TIMERA1_VECTOR
//__interrupt void TimerA_ISR(void)
__interrupt void Timer_A (void)
{
  TACTL &= ~TAIFG;
  P1OUT ^= BIT7;                           
  goToBootCounter++;
  if(goToBootCounter > 10){
    goToBootCounter = 0;
    TI_MSPBoot_JumpToBoot();
  }
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
    while(1);
}

