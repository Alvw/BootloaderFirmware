#include "msp430.h"
#include "TI_MSPBoot_Common.h"
#include "TI_MSPBoot_CI.h"
#include "TI_MSPBoot_AppMgr.h"

//
//  Local function prototypes
//
static void clock_init(void);
static void HW_init(void);

/******************************************************************************
 *
 * @brief   Main function
 *  - Initializes the MCU
 *  - Selects whether to run application or bootloader
 *  - If bootloader:
 *      - Initializes the peripheral interface
 *      - Waits for a command
 *      - Sends the corresponding response
 *  - If application:
 *      - Jump to application
 *
 *  @note   USI interrupts are enabled after this function
 * @return  none
 *****************************************************************************/
int main_boot( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

   /* 
    * Important Note: 
    *   MSPBoot assumes that proper conditions are met when executing after reset. 
    *   It’s important to remark that DCO must not be changed until VCC>=VCC(min), 
    *   and a minimum of 2.2V is required for Flash Program/Erase operations.
    *   An optional software delay can be added for applications having a slow 
    *   voltage ramp-up or an external voltage supervisor is suggested
    *   
    */
    //  __delay_cycles(10000);  // Uncomment this line and adjust number of delay 
                                // cycles according to the application voltage ramp

    // Initialize MCU
    HW_init();
    clock_init();
//------------------------------
    P1DIR |= BIT7;
    for(int i = 0; i < 5; i++){
      __delay_cycles(500000);
      P1OUT ^= BIT7;
    }
    P1OUT &= ~BIT7;
//-----------------------------------------

    // Validate the application and jump if needed
    if (TI_MSPBoot_AppMgr_ValidateApp() == TRUE_t)
        TI_MSPBoot_APPMGR_JUMPTOAPP();

    TI_MSPBoot_CI_Init();      // Initialize the Communication Interface

//BlueTooth enable pin p 4.7
  P4DIR |= BIT7;
  P4OUT |= BIT7;
  //RF reset pin
  P3DIR |= BIT7;
  P3OUT |= BIT7;

    while(1)
    {
        // Poll PHY and Data Link interface for new packets
        TI_MSPBoot_CI_PHYDL_Poll();

        // If a new packet is detected, process it
        if (TI_MSPBoot_CI_Process() == RET_JUMP_TO_APP)
        {
            // If Packet indicates a jump to App
            TI_MSPBoot_AppMgr_JumpToApp();
        }
#ifdef NDEBUG
        // Feed the dog every ~1000ms
        WATCHDOG_FEED();
#endif
    }


}

/******************************************************************************
 *
 * @brief   Initializes the MSP430 Clock
 *
 * @return  none
 *****************************************************************************/
//inline static void clock_init(void)
static void clock_init(void)
{
#if (MCLK==1000000)
    // Validate the calibration values (only in Debug)
    ASSERT_H((CALBC1_1MHZ !=0xFF) && (CALDCO_1MHZ != 0xFF));

    BCSCTL1 = CALBC1_1MHZ;              // Set DCO to default of 1Mhz
    DCOCTL = CALDCO_1MHZ;
#elif (MCLK==4000000)
    // Validate the calibration values (only in Debug)
    ASSERT_H((CALBC1_8MHZ !=0xFF) && (CALDCO_8MHZ != 0xFF));

    BCSCTL1 = CALBC1_8MHZ;              // Set DCO to default of 8Mhz
    DCOCTL = CALDCO_8MHZ;
    BCSCTL2 = DIVM_1 | DIVS_1;          // MCLK/SMCLK = 8Mhz/2 = 4Mhz
    #elif (MCLK==8000000)
    // Validate the calibration values (only in Debug)
    ASSERT_H((CALBC1_8MHZ !=0xFF) && (CALDCO_8MHZ != 0xFF));

    BCSCTL1 = CALBC1_8MHZ;              // Set DCO to default of 8Mhz
    DCOCTL = CALDCO_8MHZ;
#else
#error "Please define a valid MCLK or add configuration"
#endif
    BCSCTL3 = LFXT1S_2;                // LFXT1 = VLO
    IFG1 &= ~(OFIFG);                   // Clear OSCFault flag

}


/******************************************************************************
 *
 * @brief   Initializes the basic MCU HW
 *
 * @return  none
 *****************************************************************************/
static void HW_init(void)
{
    // The following routines disable interrupts which are cleared by POR but
    // not by a PUC. In G2xx, these are: Timers, CompA, ADC10
#ifdef HW_RESET_PUC
    TA0CTL = 0x00;
    #ifdef __MSP430_HAS_T1A3__
    TA1CTL = 0x00;
    #endif
    #ifdef __MSP430_HAS_CAPLUS__
    CACTL1 = 0x00;
    #endif
    #ifdef __MSP430_HAS_ADC10__
    ADC10CTL0 = 0x00;
    #endif
#endif
}
