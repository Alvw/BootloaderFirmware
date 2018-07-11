/*
 * \file   main.c
 *
 * \brief  Sample application for G2553 using MSPBoot
 *      This example places application in the appropiate area
 *      of memory (check linker file) and shows how to use redirected 
 *      interrupts 
*/
/* --COPYRIGHT--,BSD
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
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

    // Toggle LED1 in P1.0
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    __delay_cycles(500000);
    P1OUT &= ~BIT0;
    __delay_cycles(500000);
    P1OUT |= BIT0;
    __delay_cycles(500000);
    P1OUT &= ~BIT0;

    // Start P1.3 (S2 button) as interrupt with pull-up
    P1OUT |= BIT3;
    P1REN |= BIT3;
    P1IES |= BIT3;
    P1IE |= BIT3;

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
  P1OUT ^= 0x01;                            // Toggle P1.0
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
__interrupt void P1_Isr(void)
{
    P1IFG = 0;
    TI_MSPBoot_JumpToBoot();
}

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

