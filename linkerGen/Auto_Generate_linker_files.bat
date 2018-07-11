@echo off


REM ********** G2553 UART**************
REM Linker file for G2553_UART using 1KB
REM Flash Start: 0xC000					0x8000
REM Interrupt Vector start address: 0xFFE0
REM Bootloader start address: 0xFC00 (1KB)		FA00(1.5k)
REM Size of proxy table: 48 (12 vectors x 4)
REM Size of vector functions shared between Boot/App : 2x3=6	0
REM RAM Start: 0x200
REM RAM End: 0x3FF					0x5ff
REM Stack Size: 0x50
REM Info memory used by Bootloader Start: 0x1000
REM Info memory used by Bootloader End: 0x10BF
echo -----------------------------------------------------------------------------
echo Linker file for G2553_UART using 1KB
perl MSPBootLinkerGen.pl -file lnk_msp430G2553_UART_1KB -dev MSP430G2553 -params 0x8000 0xFFE0 0xFA00 48 0 0x200 0x5FF 0x50 0x1000 0x10BF
echo -----------------------------------------------------------------------------
copy /Y output\lnk_msp430G2553_UART*_Boot.xcl ..\Target\G2553_UART\IAR\MSPBoot\Config\
copy /Y output\lnk_msp430G2553_UART*_App.xcl ..\Target\G2553_UART\IAR\App1_MSPBoot\Config\
copy /Y output\lnk_msp430G2553_UART*_App.xcl ..\Target\G2553_UART\IAR\App2_MSPBoot\Config\

