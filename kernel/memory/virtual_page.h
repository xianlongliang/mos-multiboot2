#pragma once
#include <std/stdint.h>

constexpr auto PAGE_SIZE_4K = 0x1000;
// pml4 at 0x9000
const uint64_t PML4E_ADDR = 0x9000;

struct Page_PML4E
{
    uint64_t P : 1;          // whether present in physical memory
    uint64_t R_W : 1;        // 0 read only, 1 read write
    uint64_t U_S : 1;        // 0 kernel only, 1 all CPLs are allowed
    uint64_t PWT : 1;        // Page-Level Write Through
    uint64_t PCD : 1;        // Page-Level Cache Disable
    uint64_t A : 1;          // whether it has been accessed ?
    uint64_t IGN : 1;        // IGNORED
    uint64_t MBZ : 2;        // must be zero
    uint64_t AVL : 3;        // Available to Software, just set it to 0
    uint64_t PDPE : 40;      // the next level address
    uint64_t Available : 11; // Available to Software, just set it to 0
    uint64_t NX : 1;         // No Execute
};

struct Page_PDPE
{
    uint64_t P : 1;          // whether present in physical memory
    uint64_t R_W : 1;        // 0 read only, 1 read write
    uint64_t U_S : 1;        // 0 kernel only, 1 all CPLs are allowed
    uint64_t PWT : 1;        // Page-Level Write Through
    uint64_t PCD : 1;        // Page-Level Cache Disable
    uint64_t A : 1;          // whether it has been accessed ?
    uint64_t IGN : 1;        // IGNORED
    uint64_t SIZE : 1;       // 0 for 4k
    uint64_t IGN2 : 1;       // IGNORED
    uint64_t AVL : 3;        // Available to Software, just set it to 0
    uint64_t NEXT : 40;      // the next level address
    uint64_t Available : 11; // Available to Software, just set it to 0
    uint64_t NX : 1;         // No Execute
};

using Page_PDE = Page_PDPE;

struct Page_PTE
{
    uint64_t P : 1;         // whether present in physical memory
    uint64_t R_W : 1;       // 0 read only, 1 read write
    uint64_t U_S : 1;       // 0 kernel only, 1 all CPLs are allowed
    uint64_t PWT : 1;       // Page-Level Write Through
    uint64_t PCD : 1;       // Page-Level Cache Disable
    uint64_t A : 1;         // whether it has been accessed ?
    uint64_t D : 1;         // dirty
    uint64_t PAT : 1;       // Page-Attribute Table
    uint64_t G : 1;         // IGNORED
    uint64_t AVL : 3;       // Available to Software, just set it to 0
    uint64_t PPBA : 40;     // Physical-Page Base Address
    uint64_t Available : 7; // Available to Software, just set it to 0
    uint64_t PKE : 4;       // Usage depends on CR4.PKE
    uint64_t NX : 1;        // No Execute
};

