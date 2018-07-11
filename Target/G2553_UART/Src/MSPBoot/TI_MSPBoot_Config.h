#ifndef __TI_MSPBoot_CONFIG_H__
#define __TI_MSPBoot_CONFIG_H__

//
// Include files
//
#define MCLK                    (8000000L)  /*! MCLK Frequency in Hz */

/*! Watchdog feed sequence */
#define WATCHDOG_FEED()         {WDTCTL = (WDTPW+WDTCNTCL+WDTSSEL+WDTIS0);}
/*! Hardware entry condition in order to force bootloader mode */
#define HW_ENTRY_CONDITION      (!(P1IN & BIT3))
/*! HW_RESET_BOR: Force a software BOR in order to reset MCU. 
     Not all MCUs support this funcitonality. Check datasheet/UG for details.
    HW_RESET_PUC: Force a PUC in order to reset MCU.
     An invalid write to watchdog will force the PUC.
 */
#define HW_RESET_PUC

//
// Configuration MACROS
//
/* MSPBoot BSL-based:
 *  If MSPBoot_BSL is defined for the project (in this file or in project 
 *  preprocessor options), the following settings will apply:
 *
 *  NDEBUG = defined: Debugging is disabled, ASSERT_H function is ignored,
 *      GPIOs are not used for debugging
 *
 *  CONFIG_APPMGR_APP_VALIDATE =2: Application is validated by checking its CRC-16.
 *      An invalid CRC over the whole Application area will keep the mcu in MSPBoot
 *
 *  CONFIG_MI_MEMORY_RANGE_CHECK = defined: Address range of erase and Write
 *      operations are validated. BSL-based commands can write/erase any area
 *      including MSPBoot, but this defition prevents modifications to area 
 *      outside of Application
 *
 *  CONFIG_CI_PHYDL_COMM_SHARED = defined: Communication interface can be used 
 *      by application. Application can call MSPBoot initialization and poll 
 *      routines to use the same interface.
 *
 *  CONFIG_CI_PHYDL_ERROR_CALLBACK = undefined: The communication interface  
 *      doesn't report errors with a callback
 *
 *  CONFIG_CI_PHYDL_I2C_TIMEOUT = undefined: The communication interface doesn't 
 *      detect timeouts. Only used with I2C
 *
 *  CONFIG_CI_PHYDL_START_CALLBACK = undefined: Start Callback is not required
 *      by the UART. 
 *
 *  CONFIG_CI_PHYDL_STOP_CALLBACK = undefined: Stop callback is not needed
 *      and is not implemented to save flash space. 
 *
 *  CONFIG_CI_PHYDL_I2C_SLAVE_ADDR = 0x40. I2C slave address of this device 
 *      is 0x40. Only used with I2C
 *
 *  CONFIG_CI_PHYDL_UART_BAUDRATE = 9600. UART baudrate is 9600. Only used with
 *      UART
 */
#if defined(MSPBoot_BSL)
#   define NDEBUG
#   define CONFIG_APPMGR_APP_VALIDATE    (2)
#   define CONFIG_MI_MEMORY_RANGE_CHECK
#   undef CONFIG_CI_PHYDL_COMM_SHARED
#   undef CONFIG_CI_PHYDL_ERROR_CALLBACK
#   ifdef MSPBoot_CI_UART
#       undef CONFIG_CI_PHYDL_START_CALLBACK
#       undef CONFIG_CI_PHYDL_STOP_CALLBACK
#       define CONFIG_CI_PHYDL_UART_BAUDRATE        (460800)
#   endif
#else
#error "Define a proper configuration in TI_MSPBoot_Config.h or in project preprocessor options"
#endif

#endif            //__TI_MSPBoot_CONFIG_H__
