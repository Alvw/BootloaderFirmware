/*
 * \file   TI_MSPBoot_Mgr_Vectors_G2553.c
 *
 * \brief  Vector manager for G2553
 *         This file should be added to applications based on MSPBoot in order
 *         to enable redirected interrupts
 *         Application interrupts must be added to the proxy table
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

// Global variables
/*! Password sent by Application to force boot mode. This variable is in a fixed
    location and should keep same functionality and location in Boot and App */
#ifdef __IAR_SYSTEMS_ICC__
#           pragma location="RAM_PASSWORD"
    __no_init uint16_t  PassWd;
#       elif defined (__TI_COMPILER_VERSION__)
extern uint16_t  PassWd;
#endif

/*! Status and Control byte. This variable is in a fixed
    location and should keep same functionality and location in Boot and App */
#ifdef __IAR_SYSTEMS_ICC__
#           pragma location="RAM_STATCTRL"
    __no_init uint8_t  StatCtrl;
#       elif defined (__TI_COMPILER_VERSION__)
extern uint8_t  StatCtrl;
#endif


/******************************************************************************
 *
 * @brief   Forces Boot mode
 *  Sets a bit in Status and Control byte to request Boot mode, writes a password
 *  in PassWd and forces a PUC reset
 *  Boot will start execution after PUC and detect forced Boot mode
 *
 * @return  none
 *****************************************************************************/
void TI_MSPBoot_JumpToBoot( void )
{
    PassWd = BSL_PASSWORD;      // Send password
    StatCtrl |= BOOT_APP_REQ;   // Set flag to request boot mode
    __disable_interrupt();      // Disable all interrupts
    WDTCTL = WDT_MRST_0_064;    // Set watchdog and wait for expiration
    while (1)
        ;

}

//
//  External ISR prototypes used by ProxyVectorTable
//  Add your own ISRs as shown below
//
extern __interrupt void P1_Isr(void);
extern __interrupt void Dummy_Isr (void);
extern __interrupt void TI_MSPBoot_Comm_Isr(void);


//
//  Constant tables
//
/*! This is a "proxy" interrupt table which is used by the bootloader to jump to
    each interrupt routine.
    It always resides in the same location.
    It contains a BRA instruction (0x4030) followed by the address of each
    vector routine.
    Note that the number and location of each vector should match the declaration
    in Boot's Vector_Table (TI_MSPBoot_VecRed_G2xxxx.c)
    Unimplemented vectors are removed (and unused vectors can be removed too)
    to save flash space
    The following table matches the vectors for G2553
*/
#   ifdef __IAR_SYSTEMS_ICC__
#       pragma location="APP_PROXY_VECTORS"
__root const uint16_t ProxyVectorTable[] =
#   elif defined (__TI_COMPILER_VERSION__)
#       pragma DATA_SECTION(ProxyVectorTable, ".APP_PROXY_VECTORS")
#       pragma RETAIN(ProxyVectorTable)
const uint16_t ProxyVectorTable[] =
#   endif
{
        0x4030, (uint16_t) P1_Isr,              // APP_PROXY_VECTOR(0) P1
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(1) P2
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(2) ADC10
        0x4030, (uint16_t) TI_MSPBoot_Comm_Isr,   // APP_PROXY_VECTOR(3) USCI I2C TX/RX
        0x4030, (uint16_t) TI_MSPBoot_Comm_Isr, // APP_PROXY_VECTOR(4) USCI I2C STAT
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(5) TA0_1
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(6) T0_0
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(7) WDT
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(8) COMP_A
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(9) TA1_1
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(10) TA1_0
        0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(11) NMI
};
