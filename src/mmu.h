#ifndef __MMU_H
#define __MMU_H

/* Setting TCR_EL1 */

// T1SZ, bits [21:16], The size offset of the memory region addressed by TTBR1_EL1. 
// The region size is 2^(64-T1SZ) bytes.
#define TCR_T1SZ ((64 - 48) << 16)

// T0SZ, bits [5:0], The size offset of the memory region addressed by TTBR0_EL1. 
// The region size is 2(64-T0SZ) bytes.
#define TCR_T0SZ ((64 - 48) <<  0)

// TG1, bits[31:30], Granule size for the TTBR1_EL1.
// 0b10 : 4KB
#define TCR_TG1 (0b10 << 30)

// TG0, bits [15:14], Granule size for the TTBR0_EL1.
// 0b00 : 4KB
#define TCR_TG0 (0b00 << 14)

#define TCR_CONFIG_4KB (TCR_TG1 | TCR_TG0)
#define TCR_CONFIG_REGION_48bit (TCR_T1SZ | TCR_T0SZ)
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

/* Setting MAIR_EL1 */
#define MAIR_DEVICE_nGnRnE 0b00000000

// (Normal memory, Outer Non-cacheable | Normal memory, Inner Non-cacheable)
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1
#define MAIR_EL1_VALUE (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
                       (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \


/* Setting TCR_EL1 */
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_PAGE  0b11
#define PD_ACCESS_FLAG          (1 << 10)

// bit[10] : access flag, bit[4:2] : MAIR index, bit[1:0] : next level type
#define PGD0_ATTR               PD_TABLE // Lower attributes is ignored
#define PUD0_ATTR               PD_TABLE // Lower attributes is ignored
#define PUD1_ATTR               (PD_ACCESS_FLAG | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define PMD_ATTR                PD_TABLE // Lower attributes is ignored
#define PTE_DEVICE_ATTR         (PD_ACCESS_FLAG | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
#define PTE_NORMAL_ATTR         (PD_ACCESS_FLAG | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)

#endif