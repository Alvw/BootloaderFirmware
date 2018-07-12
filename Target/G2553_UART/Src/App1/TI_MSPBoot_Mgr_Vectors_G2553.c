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
//extern __interrupt void P1_Isr(void);
extern __interrupt void Timer_A (void);
extern __interrupt void Dummy_Isr (void);


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
    0x4030, (uint16_t) Dummy_Isr,              // APP_PROXY_VECTOR(0) P1
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(1) P2
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(2) ADC10
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(3) USCI I2C TX/RX
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(4) USCI I2C STAT
    0x4030, (uint16_t) Timer_A,           // APP_PROXY_VECTOR(5) TA0_1
    0x4030, (uint16_t) Dummy_Isr,             // APP_PROXY_VECTOR(6) T0_0
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(7) WDT
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(8) COMP_A
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(9) TA1_1
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(10) TA1_0
    0x4030, (uint16_t) Dummy_Isr,           // APP_PROXY_VECTOR(11) NMI
};
