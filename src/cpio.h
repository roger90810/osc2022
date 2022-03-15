#ifndef __CPIO_H
#define __CPIO_H

#include "string.h"
#include "uart.h"
// #define CPIO_BASE         ((volatile unsigned int*)(0x20000000))
extern uint32_t CPIO_BASE;

typedef struct {
    char c_magic[6];    // The string "070701".
    char c_ino[8];      // i-node number
    char c_mode[8];     // regular permissions and the file type
    char c_uid[8];      // user id of owner
    char c_gid[8];      // group id of owner
    char c_nlink[8];    // Number of links to the file.
    char c_mtime[8];    // Modification time of the file, epoch_s
    char c_filesize[8]; // The size of the file, limit 4GB
    char c_devmajor[8]; 
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8]; // The numberof bytes in the pathname that follows the header, includes terminating NUL.
    char c_check[8];    // always set to zero by writers
} cpio_newc_header_t;

void cpio_ls();
void cpio_cat(const char*);
#endif