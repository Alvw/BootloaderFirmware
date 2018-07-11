# --COPYRIGHT--,BSD
#  Copyright (c) 2012, Texas Instruments Incorporated
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
# 
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# --/COPYRIGHT--

#Perl script used to generate linker files for MSPBoot
#   Check help for usage
#

use strict;
use warnings;
use POSIX qw/strftime/;
use Getopt::Long;

print( "\n MSPBootLinkerGen.pl V1.0 \n");
print( "Texas Instruments Inc 2014 \n");

my $filename_tempBootIAR  = "template_Boot_IAR.tmpl";
my $filename_tempAppIAR  = "template_App_IAR.tmpl";
my $filename_tempBootCCS  = "template_Boot_CCS.tmpl";
my $filename_tempAppCCS  = "template_App_CCS.tmpl";

my $filename;
my $device;
my $dual_size;
my $int_file;
my @params;
GetOptions ("file=s" => \$filename,             # string
              "device=s{1}"   => \$device,      # string
              "dual_size=s{1}" => \$dual_size,  # string
              "int_table=s" => \$int_file,      # string
              "params=s{10}"  => \@params,      # array of strings
              'help|?'  => sub {help() })      # array of strings
  or die("Error in command line arguments\n");

if ((!$filename) || (!@params))
{
    print("Error in command line arguments\n");
    help();
    die;
}
    
my $date = strftime('%m-%d-%Y',localtime);

my $flash_start_addr=hex($params[0]);
my $flash_vectors_addr=hex($params[1]);
my $flash_boot_start_addr=hex($params[2]);
my $proxy_vector_size=$params[3];
my $shared_vectors_size=$params[4];
my $RAM_start_addr=hex($params[5]);
my $RAM_end_addr=hex($params[6]);
my $STACK_Size=hex($params[7]);
my $BootInfo_Start_Addr=hex($params[8]);
my $BootInfo_End_Addr=hex($params[9]);
my $dual_start_addr;
if ($dual_size)
{
    $dual_start_addr= $flash_start_addr;
    $flash_start_addr += hex($dual_size);
}

#open templates
open(my $FBIT, '<', $filename_tempBootIAR) or die "Could not open file '$filename_tempBootIAR' $!";
open(my $FAIT, '<', $filename_tempAppIAR) or die "Could not open file '$filename_tempAppIAR' $!";
open(my $FBCT, '<', $filename_tempBootCCS) or die "Could not open file '$filename_tempBootCCS' $!";
open(my $FACT, '<', $filename_tempAppCCS) or die "Could not open file '$filename_tempAppCCS' $!";

#Specify the name of output files
my $outputdir = './output/';
mkdir $outputdir unless -d $outputdir;
my $filename_OutBootIAR = $outputdir.$filename.'_Boot.xcl';
my $filename_OutAppIAR = $outputdir.$filename.'_App.xcl';
my $filename_OutBootCCS = $outputdir.$filename.'_Boot.cmd';
my $filename_OutAppCCS = $outputdir.$filename.'_App.cmd';

#Open output files
open(my $FBIO, '>', $filename_OutBootIAR) or die "Could not open file '$filename_OutBootIAR' $!";
open(my $FAIO, '>', $filename_OutAppIAR) or die "Could not open file '$filename_OutAppIAR' $!";
open(my $FBCO, '>', $filename_OutBootCCS) or die "Could not open file '$filename_OutBootCCS' $!";
open(my $FACO, '>', $filename_OutAppCCS) or die "Could not open file '$filename_OutAppCCS' $!";

my @temp_text;

##### Replace text in Boot IAR file
{
    @temp_text = <$FBIT>;
    replacetextinFile('<DATE>', $date);
    replacetextinFile('<FLASH_START_ADDR>', gethex($flash_start_addr));
    if ($device){ replacetextinFile('<DEVICE>', $device); }
    replacetextinFile('<FLASH_VECTORS_ADDR>', gethex($flash_vectors_addr));
    replacetextinFile('<FLASH_BOOT_START_ADDR>', gethex($flash_boot_start_addr));
    replacetextinFile('<PROXY_VECTOR_SIZE>', gethex($proxy_vector_size));
    replacetextinFile('<SHARED_VECTORS_SIZE>', gethex($shared_vectors_size));
    replacetextinFile('<RAM_START_ADDR>', gethex($RAM_start_addr));
    replacetextinFile('<RAM_END_ADDR>', gethex($RAM_end_addr));
    replacetextinFile('<STACK_SIZE>', gethex($STACK_Size));
    replacetextinFile('<BOOTINFO_START_ADDR>', gethex($BootInfo_Start_Addr));
    replacetextinFile('<BOOTINFO_END_ADDR>', gethex($BootInfo_End_Addr));
    if ($dual_size) {
        replacetextinFile('<BOOT1>', "\n// Download area: ".gethex($dual_start_addr).'-'.gethex($flash_start_addr-1)."\n".
                                       '// App area     : '.gethex($flash_start_addr).'-'.gethex($flash_boot_start_addr-1)."\n".
                                       '// Boot area    : '.gethex($flash_boot_start_addr).'-'.gethex(0xFFFF)."\n".
                                       '    -D_FLASHDOWN_START='.gethex($dual_start_addr));
        replacetextinFile('<BOOT2>', "
    // Addresses for Dual Image support
        -D_AppChecksumDown=_FLASHDOWN_START
        -D_AppChecksumDown_8=(_FLASHDOWN_START+2)
        -D_App_StartDown=(_FLASHDOWN_START+3)
        -D_App_EndDown=(_FLASH_START-1)");

    }else{
        replacetextinFile('<BOOT1>', "");
        replacetextinFile('<BOOT2>', "");
    }
    print $FBIO @temp_text ;
}

##### Replace text in App IAR file
{
    @temp_text = <$FAIT>;
    replacetextinFile('<DATE>', $date);
    replacetextinFile('<FLASH_START_ADDR>', gethex($flash_start_addr));
    if ($device){ replacetextinFile('<DEVICE>', $device); }
    replacetextinFile('<FLASH_VECTORS_ADDR>', gethex($flash_vectors_addr));
    replacetextinFile('<FLASH_BOOT_START_ADDR>', gethex($flash_boot_start_addr));
    replacetextinFile('<PROXY_VECTOR_SIZE>', gethex($proxy_vector_size));
    replacetextinFile('<SHARED_VECTORS_SIZE>', gethex($shared_vectors_size));
    replacetextinFile('<RAM_START_ADDR>', gethex($RAM_start_addr));
    replacetextinFile('<RAM_END_ADDR>', gethex($RAM_end_addr));
    replacetextinFile('<STACK_SIZE>', gethex($STACK_Size));
    replacetextinFile('<BOOTINFO_START_ADDR>', gethex($BootInfo_Start_Addr));
    replacetextinFile('<BOOTINFO_END_ADDR>', gethex($BootInfo_End_Addr));
    if ($dual_size) {
        replacetextinFile('<BOOT1>', "\n// Download area: ".gethex($dual_start_addr).'-'.gethex($flash_start_addr-1)."\n".
                                       '// App area     : '.gethex($flash_start_addr).'-'.gethex($flash_boot_start_addr-1)."\n".
                                       '// Boot area    : '.gethex($flash_boot_start_addr).'-'.gethex(0xFFFF)."\n".
                                       '    -D_FLASHDOWN_START='.gethex($dual_start_addr));
    }else{
        replacetextinFile('<BOOT1>', "");
    }

    print $FAIO @temp_text ;
}
    
    
##### Replace text in Boot CCS file
{
    @temp_text = <$FBCT>;
    replacetextinFile('<DATE>', $date);
    if ($device){ replacetextinFile('<DEVICE>', $device); }
    replacetextinFile('<RAM_START_ADDR>', gethex($RAM_start_addr));
    replacetextinFile('<RAM_END_ADDR>', gethex($RAM_end_addr));
    replacetextinFile('<RAM_PASSWD_ADDR>', gethex($RAM_start_addr+0));
    replacetextinFile('<RAM_STATCTRL_ADDR>', gethex($RAM_start_addr+2));
    replacetextinFile('<RAM_CISM_ADDR>', gethex($RAM_start_addr+3));
    replacetextinFile('<RAM_CALLBACKPTR_ADDR>', gethex($RAM_start_addr+4));
    replacetextinFile('<RAM_UNUSED_ADDR>', gethex($RAM_start_addr+6));

    replacetextinFile('<FLASH_START_ADDR>', gethex($flash_start_addr));
    replacetextinFile('<FLASH_BOOT_START_ADDR>', gethex($flash_boot_start_addr));
    replacetextinFile('<FLASH_VECTORS_ADDR>', gethex($flash_vectors_addr));
    replacetextinFile('<SHARED_VECTORS_SIZE>', $shared_vectors_size);
    my $shared_vector_addr = $flash_vectors_addr -$shared_vectors_size;
    replacetextinFile('<SHARED_VECTORS_ADDR>', gethex($shared_vector_addr));

    replacetextinFile('<PROXY_VECTOR_SIZE>', gethex($proxy_vector_size));
    replacetextinFile('<PROXY_VECTOR_ADDR>', gethex($flash_boot_start_addr -2 -$proxy_vector_size));

    replacetextinFile('<RAM_LEN>', gethex($RAM_end_addr - ($RAM_start_addr+6)+1 ));
    replacetextinFile('<BOOTINFO_START_ADDR>', gethex($BootInfo_Start_Addr));
    replacetextinFile('<BOOTINFO_LEN>', gethex($BootInfo_End_Addr - $BootInfo_Start_Addr+1));
    replacetextinFile('<FLASH_BOOT_LEN>', gethex($shared_vector_addr - $flash_boot_start_addr));
    replacetextinFile('<FLASH_VECTORS_LEN>', gethex(0xFFFE - $flash_vectors_addr));

    if ($dual_size) {
        replacetextinFile('<BOOT1>', "\n/* Download area: ".gethex($dual_start_addr).'-'.gethex($flash_start_addr-1)."*/\n".
                                       '/* App area     : '.gethex($flash_start_addr).'-'.gethex($flash_boot_start_addr-1)."*/\n".
                                       '/* Boot area    : '.gethex($flash_boot_start_addr).'-'.gethex(0xFFFF)."*/\n".
                                       '_FLASHDOWN_START = 0x'.gethex($dual_start_addr).'; /* Start if download area */');
        replacetextinFile('<BOOT2>', "
    // Addresses for Dual Image support in download ares
        _AppChecksumDown = (_FLASHDOWN_START);      
        _AppChecksumDown_8 = (_FLASHDOWN_START+2);  
        _App_StartDown = (_FLASHDOWN_START+3);
        _App_EndDown = (__Flash_Start-1);");

    }else{
        replacetextinFile('<BOOT1>', "");
        replacetextinFile('<BOOT2>', "");
    }
    print $FBCO @temp_text ;    
}


##### Replace text in App CCS file
{
    @temp_text = <$FACT>;
    replacetextinFile('<DATE>', $date);
    if ($device){ replacetextinFile('<DEVICE>', $device); }
    replacetextinFile('<RAM_START_ADDR>', gethex($RAM_start_addr));
    replacetextinFile('<RAM_END_ADDR>', gethex($RAM_end_addr));
    replacetextinFile('<RAM_PASSWD_ADDR>', gethex($RAM_start_addr+0));
    replacetextinFile('<RAM_STATCTRL_ADDR>', gethex($RAM_start_addr+2));
    replacetextinFile('<RAM_CISM_ADDR>', gethex($RAM_start_addr+3));
    replacetextinFile('<RAM_CALLBACKPTR_ADDR>', gethex($RAM_start_addr+4));
    replacetextinFile('<RAM_UNUSED_ADDR>', gethex($RAM_start_addr+6));

    replacetextinFile('<FLASH_START_ADDR>', gethex($flash_start_addr));
    replacetextinFile('<FLASH_BOOT_START_ADDR>', gethex($flash_boot_start_addr));
    replacetextinFile('<FLASH_VECTORS_ADDR>', gethex($flash_vectors_addr));
    replacetextinFile('<SHARED_VECTORS_SIZE>', $shared_vectors_size);
    my $shared_vector_addr = $flash_vectors_addr -$shared_vectors_size;
    replacetextinFile('<SHARED_VECTORS_ADDR>', gethex($shared_vector_addr));

    my $proxy_vector_addr = (($flash_boot_start_addr -2) -$proxy_vector_size);
    my $app_reset_addr = ($flash_boot_start_addr - 2);
    my $app_vector_table = "";
    
    # Special case when using interrupt vectors in CCS
    if ($int_file)
    {
        my $count=0;
        for (my $ii=$proxy_vector_addr; $ii < $app_reset_addr ; $ii +=2)
        {
            $app_vector_table = $app_vector_table."INT".sprintf("%02d", $count++)."            : origin = 0x".gethex($ii).", length = 0x0002\n    ";
        }
    }
    else
    {
        $app_vector_table = "APP_PROXY_VECTORS       : origin = 0x<PROXY_VECTOR_ADDR>, length = <PROXY_VECTOR_SIZE>";
    }

    replacetextinFile('<APP_VECTORS_MEM>', $app_vector_table);
    replacetextinFile('<PROXY_VECTOR_SIZE>', ($proxy_vector_size));
    replacetextinFile('<PROXY_VECTOR_ADDR>', gethex($proxy_vector_addr));
    

    replacetextinFile('<RAM_LEN>', gethex($RAM_end_addr - ($RAM_start_addr+6)+1 ));
    replacetextinFile('<BOOTINFO_START_ADDR>', gethex($BootInfo_Start_Addr));
    replacetextinFile('<BOOTINFO_LEN>', gethex($BootInfo_End_Addr - $BootInfo_Start_Addr+1));
    replacetextinFile('<APP_FLASH_START>', gethex($flash_start_addr+3));
    replacetextinFile('<APP_FLASH_LEN>', gethex($proxy_vector_addr-($flash_start_addr+3)));
    replacetextinFile('<FLASH_BOOT_LEN>', gethex($shared_vector_addr - $flash_boot_start_addr));
    replacetextinFile('<APP_RESET_VECTOR>', gethex($app_reset_addr));
    
    # Special case when using interrupt vectors in CCS
    my $app_vector_sections;
    if ($int_file)
    {
        open(my $interrupt_vectors, '<', $int_file) or die "Could not open file '$int_file' $!";
        #copy file to array
        $app_vector_sections = do { local $/; <$interrupt_vectors> };
    }
    else
    {
        $app_vector_sections = ".APP_PROXY_VECTORS : {} > APP_PROXY_VECTORS /* INTERRUPT PROXY TABLE            */";
    }
    replacetextinFile('<APP_VECTORS_SECTIONS>', $app_vector_sections);

    if ($dual_size) {
        replacetextinFile('<BOOT1>', "\n/* Download area: ".gethex($dual_start_addr).'-'.gethex($flash_start_addr-1)."*/\n".
                                       '/* App area     : '.gethex($flash_start_addr).'-'.gethex($flash_boot_start_addr-1)."*/\n".
                                       '/* Boot area    : '.gethex($flash_boot_start_addr).'-'.gethex(0xFFFF)."*/\n".
                                       '_FLASHDOWN_START = 0x'.gethex($dual_start_addr).'; /* Start if download area */');
    }else{
        replacetextinFile('<BOOT1>', "");
    }
    print $FACO @temp_text ;    
}

close $FBIO; close $FBIT; 
close $FAIO; close $FAIT; 
close $FBCO; close $FBCT; 
close $FACO; close $FACT; 

#param 0 = text to replace
#param 1 = new text
#param 2 = file
sub replacetextinFile{
    my ($in_text, $out_text) = @_;
    for (@temp_text)
    {
        s/$in_text/$out_text/g;
    }
}

sub gethex{
    return sprintf("%X", $_[0]);
}

#Help 
sub help{ 
    print "
Generates linker files for MSPBoot

MSPBootLinkerGen.pl [-help] -file <filename> -device <dev> 
                    [-dual_size <di_size>] 
                    [-int_table <int_file>] 
                     -params <mem_start> <int_vect> <boot_start> <proxy_size> 
                     <sv_size> <RAM_start> <RAM_end> <stack_size> <info_start> 
                     <info_end>
        
  -file         Specifies the prefix of output files. 
   <filename>   Prefix of output files. The following files will be generated:
                ./output/<filename>_Boot.xcl(linker file for Bootloader in IAR)
                ./output/<filename>_App.xcl(linker file for App in IAR)
                ./output/<filename>_Boot.cmd(linker file for Bootloader in CCS)
                ./output/<filename>_App.cmd(linker file for App in CCS).
  -device       Specifies the device being used.
   <dev>        Device used (i.e. MSP430G2553).
  -dual_size    If enabled, the output files include definitions supporting 
                dual image.
   <di_size>    Size of each partition supporting dual image.
                i.e. 0x2000 defines a Download partition of 8KB starting from
                the start of flash, and an Application partition of 8KB 
                starting after the download partition and ending before 
                bootloader.
  -int_table    If enabled, the proxy table is not used and interrupts will be
                obtained from the specified template file. 
                This option is useful for devices like FR57xx which don't need
                vector redirection. 
                It's important to remark that IAR doesn't declare vectors in 
                the linker file, but CCS does.
   <int_file>   Name of the file including the vectors for CCS .cmd file.
  -params       10 necessary parameters required to generate linker files.
   <mem_start>  Start address of Flash/FRAM (i.e. 0xC000 for G2553)
   <int_vect>   Address of interrupt vector table (i.e. 0xFFE0 for G2553)
   <boot_start> Start address of Bootloader (i.e. 0xFC00 for 1KB)
   <proxy_size> Size of the proxy table (i.e. 48 for 12 vectors using 4B each)
   <sv_size>    Size of the shared vectors (i.e. 4 for 2 vectors of 2B each)
   <RAM_start>  Start address of RAM (i.e. 0x200 for G2553)
   <RAM_end>    End address of RAM (i.e. 0x3FF for G2553)
   <stack_size> Size of the stack (i.e. 0x50 for G2553)
   <info_start> Start address of info memory used for bootloader (i.e. 0x1000)
   <info_end>   End address of info memory used for bootloader (i.e. 0x10BF)
  -help         This help.
            
EXAMPLE: 
The following command generates linker files for G2553 with the following 
memory map:
RAM: 0x200-0x3FF
    Stack size: 0x50
Application: 0xC000-0xFBFF
    App_code: 0xC000-0xFBCD
    Proxy_Table: 0xFBCE-0xFBFD
    APP_Reset_vector: 0xFBFE-0xFBFF
Bootloader: 0xFC00-0xFFFF + 0x1000-0x10BF
    Boot_Code: 0xFC00-0xFFDB
    Shared_vectors: 0xFFDC-0xFFDF
    Vectors: 0xFFE0-0xFFFD
    Reset_Vectors: 0xFFFE-0xFFFF
    
perl MSPBootLinkerGen.pl -file lnk_msp430G2553_I2C_1KB -dev MSP430G2553 
-params 0xC000 0xFFE0 0xFC00 48 4 0x200 0x3FF 0x50 0x1000 0x10BF 

"; 
    exit; 
} 
