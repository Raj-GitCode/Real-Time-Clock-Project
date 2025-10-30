// defines.h
#ifndef DEFINES_H
#define DEFINES_H

#define WRITEBYTE(REG,BIT,VAL)   REG = (REG & ~(0xFF<<BIT)) | ((VAL&0xFF)<<BIT)

#endif
