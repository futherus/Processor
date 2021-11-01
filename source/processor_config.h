#ifndef PROCESSOR_CONFIG_H
#define PROCESSOR_CONFIG_H

//#define CPU_VERBOSE
//#define CPU_DUMP

/// Max amount of arguments for command
const unsigned char ARGS_CAP = 0x6;

/// Max amount of lexems in one argument
const unsigned char LEXS_CAP = 0x6;

/// Max amount of labels for program
const size_t LBLS_CAP = 0x20;

/// Max length of line in source file
const size_t TXT_CMD_CAP = 0x200;

/// Max lenght of binary output for line
const size_t BIN_LINE_CAP = 0x200;

/// Accesible registers rax - r*x
const unsigned char USRREGS_CAP = 0x10;

/// Argument registers  dax - d*x
const unsigned char ARGREGS_CAP = ARGS_CAP;
const unsigned char REG_dax = USRREGS_CAP; 

/// Temporary registers for system commands
const unsigned char TEMPREGS_CAP = 0x4;
const unsigned char REG_tax = USRREGS_CAP + ARGREGS_CAP;

/// Amount of registers
const unsigned char REGS_CAP = USRREGS_CAP + ARGREGS_CAP + TEMPREGS_CAP;

/// Capacity of RAM
const size_t RAM_CAP  = 0x100;

/// Capacity of VRAM
const long RESOL_X  = 128;
const long RESOL_Y  = 72;
const size_t VRAM_CAP = RESOL_X * RESOL_Y;

const size_t MEM_CAP  = RAM_CAP + VRAM_CAP / sizeof(size_t);

#endif // PROCESSOR_CONFIG_H
