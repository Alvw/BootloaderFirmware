/*
 * \file   main2.c
 *
 * \brief  Second sample application for G2553 using MSPBoot
 *      This example places application in the appropiate area
 *      of memory (check linker file), shows how to use redirected 
 *      interrupts and how to use the shared communication module
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
#include "TI_MSPBoot_Mgr_CI.h"
#include "TI_MSPBoot_Mgr_Vectors.h"

// 
// Global variables
//
static uint8_t RxByte;     /*! Byte received from I2C */
static uint8_t new_data;   /*! Flag to indicate new data reception */

//
// Function prototypes
//
#ifdef SHARED_PI
static void App_Comm_Rx(uint8_t data);
#endif

/******************************************************************************
 *
 * @brief   Main function
 *  This example application performs the following functions:
 *  - Toggle LED2 (P2.0) at startup (to indicate App2 execution)
 *  - Toggles LED2 when button S2 (P1.3) is pressed (demonstrates vector
 *      redirection)
 *  - Uses the shared I2C API from MSPBoot detecting some application commands
 *      (demonstrates MSPBoot PI sharing)
 *  - Jumps to Bootloader when the corresponding I2C command is detected
 *
 * @return  none
 *****************************************************************************/
int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // LED0 off by default
    P1DIR |= BIT0;
    P1OUT = 0;
    
    // Toggle LED1 in P2.0
    P2DIR |= BIT0;
    P2OUT  = BIT0;
    __delay_cycles(500000);
    P2OUT &= ~BIT0;
    __delay_cycles(500000);
    P2OUT |= BIT0;
    __delay_cycles(500000);
    P2OUT &= ~BIT0;

    // Set S2 (P1.3) as interrupt with internal pull-up
    P1OUT |= BIT3;
    P1REN |= BIT3;
    P1IES |= BIT3;
    P1IE |= BIT3;
    
#if (defined(SHARED_PI))
    /* If the MSPBoot supports it, initialize the shared comm interface
        Note that this function will fail if the comm is not shared and the init
        vector is invalid */
    // This example only uses the RX callbacks
    // The TX and error callback are not implemented
    // I2C callbacks are disabled
    TI_MSPBoot_Comm_Init(App_Comm_Rx, NULL, NULL);
#endif

    new_data = 0;

    while (1)
    {
        __bis_SR_register(LPM3_bits + GIE);
        __disable_interrupt();

        if (new_data ==1)
        {
            //This function demonstrates how send data to the shared Communication
            TI_MSPBoot_Comm_TxByte(RxByte); // Echo the received byte
            
            switch (RxByte)
            {
                // Process byte received from Shared Comm
                case 0xAA:
                    // Force Boot mode
                    TI_MSPBoot_JumpToBoot();
                break;
                case 0x01:
                    // Toggle LED1 (P1.0)
                    P1OUT ^= BIT0;
                break;
                case 0x02:
                    // Toggle LED2 (P2.0)
                    P2OUT ^= BIT0;
                break;
                default:
                    // Toggle both LEDs
                    P1OUT ^= (BIT0);
                    P2OUT ^= (BIT0);
                break;
            }

            new_data = 0; // Clear flag
        }
    }

}

/******************************************************************************
 *
 * @brief   P1 Interrupt service routine
 *  This routine simply toggles an LED but it shows how to declare interrupts
 *   in Application space
 *   Note that this function prototype should be accessible by TI_MSPBoot_Mgr_Vectors.c
 *   and it should be declared in ProxyVectorTable
 *
 * @return  none
 *****************************************************************************/
__interrupt void P1_Isr(void)
{
    P1IFG = 0;
    P2OUT ^= BIT0;
}

#ifdef SHARED_PI
/******************************************************************************
 *
 * @brief   RXCallback for Shared Comm interface
 *  This function demonstrates how to receive data from the shared Communication
 *  interface. Function is called by MSPBoot when a byte is received from the host
 *
 *   Note that this function prototype should be accessible by TI_MSPBoot_Mgr_PI.c
 *   and is called by TI_MSPBoot_Comm_Init
 *
 * @return  none
 *****************************************************************************/
static void App_Comm_Rx(uint8_t data)
{
    RxByte = data;
    new_data = 1;
}

#endif //SHARED_PI

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
