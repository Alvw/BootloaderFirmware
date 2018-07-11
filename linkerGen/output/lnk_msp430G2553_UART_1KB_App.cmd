/******************************************************************************/
/* LINKER COMMAND FILE FOR MSPBoot BOOTLOADER USING MSP430G2553  */
/* File generated with MSPBootLinkerGen.pl on 07-11-2018 */
/*----------------------------------------------------------------------------*/


/****************************************************************************/
/* SPECIFY THE SYSTEM MEMORY MAP                                            */
/****************************************************************************/
/* The following definitions can be changed to customize the memory map for a different device
 *   or other adjustments
 *  Note that the changes should match the definitions used in MEMORY and SECTIONS
 *
 */
/* RAM Memory Addresses */
__RAM_Start = 0x200;                 /* RAM Start */
__RAM_End = 0x5FF;                     /* RAM End */
    /* RAM shared between App and Bootloader, must be reserved */
    PassWd = 0x200;                 /* Password sent by App to force boot  mode */
    StatCtrl = 0x202;             /* Status and Control  byte used by Comm */
    CI_State_Machine = 0x203;         /*  State machine variable used by Comm */
    CI_Callback_ptr = 0x204;   /* Pointer to Comm callback structure */
    /* Unreserved RAM used for Bootloader or App purposes */
    _NonReserved_RAM_Start = 0x206; /* Non-reserved RAM */

/* Flash memory addresses */
__Flash_Start = 0x8000;             /* Start of Flash */
__Flash_End = 0xFFFF;                           /* End of Flash */
    /* Reserved Flash locations for Bootloader Area */
    __Boot_Start = 0xFA00;         /* Boot flash */
    __Boot_Reset = 0xFFFE;                          /* Boot reset vector */
    __Boot_VectorTable = 0xFFE0;      /* Boot vector table */
    __Boot_SharedCallbacks_Len = 0; /* Length of shared callbacks (2 calls =4B(msp430) or 8B(msp430x) */
    __Boot_SharedCallbacks = 0xFFE0; /* Start of Shared callbacks */
    _BOOT_APPVECTOR = __Boot_SharedCallbacks;       /* Definition for application table             */
    /* Reserved Flash locations for Application Area */
    _AppChecksum = (__Flash_Start);                 /* CRC16 of Application                         */
    _AppChecksum_8 = (__Flash_Start+2);             /* CRC8 of Application                          */
    _App_Start = (__Flash_Start+3);                 /* Application Area                             */
    _App_End = (__Boot_Start-1);                    /* End of application area (before boot)        */
    _CRC_Size = (_App_End - _App_Start +1);         /* Number of bytes calculated for CRC           */
    _App_Reset_Vector = (__Boot_Start-2);           /* Address of Application reset vector */
    _App_Proxy_Vector_Start = 0xF9CE; /* Proxy interrupt table */

/* MEMORY definition, adjust based on definitions above */
MEMORY
{
    SFR                     : origin = 0x0000, length = 0x0010
    PERIPHERALS_8BIT        : origin = 0x0010, length = 0x00F0
    PERIPHERALS_16BIT       : origin = 0x0100, length = 0x0100
    // RAM from _NonReserved_RAM_Start - __RAM_End
    RAM                     : origin = 0x206, length = 0x3FA
    INFOBOOT                : origin = 0x1000, length = 0xC0
    // Flash from _App_Start -> (APP_PROXY_VECTORS-1)
    FLASH                   : origin = 0x8003, length = 0x79CB
    // Interrupt Proxy table from  _App_Proxy_Vector_Start->(RESET-1)
    APP_PROXY_VECTORS       : origin = 0xF9CE, length = 48
    // App reset from _App_Reset_Vector
    RESET                   : origin = 0xF9FE, length = 0x0002
}

/****************************************************************************/
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY                              */
/****************************************************************************/

SECTIONS
{
    .bss        : {} > RAM                /* GLOBAL & STATIC VARS              */
    .data       : {} > RAM                /* GLOBAL & STATIC VARS              */
    .sysmem     : {} > RAM                /* DYNAMIC MEMORY ALLOCATION AREA    */
    .stack      : {} > RAM (HIGH)         /* SOFTWARE SYSTEM STACK             */

    .text       : {} >> FLASH |INFOBOOT   /* CODE                 */
    .cinit      : {} > FLASH |INFOBOOT   /* INITIALIZATION TABLES*/
    .const      : {} >> FLASH |INFOBOOT   /* CONSTANT DATA        */
    .cio        : {} > RAM                /* C I/O BUFFER                      */

    .APP_PROXY_VECTORS : {} > APP_PROXY_VECTORS /* INTERRUPT PROXY TABLE            */
    .reset       : {}               > RESET  /* MSP430 RESET VECTOR                 */
}

/****************************************************************************/
/* INCLUDE PERIPHERALS MEMORY MAP                                           */
/****************************************************************************/

-l MSP430G2553.cmd

