#include "mapping.h"
#include "virtual_page.h"
#include "physical.h"
#include "flags.h"

// kernel reserve 0x600000 ~ 0x800000 2M virtual address for storing 512 PTE
static uint8_t*kernel_pte_reserved = (uint8_t*)0x600000 + PAGE_OFFSET;

static uint8_t*pte_alloc()
{
    printk("pte_alloc\n");
    auto res = kernel_pte_reserved;
    kernel_pte_reserved = kernel_pte_reserved + PAGE_4K_SIZE;
    return res;
}

struct SwapPTE
{
    uint64_t paddr;
    uint64_t vaddr;
};

static SwapPTE swap_pte;

void vmap_init()
{
    printk("%p\n", &swap_pte);
    swap_pte = {(uint64_t)Virt_To_Phy(kernel_pte_reserved), (uint64_t)kernel_pte_reserved};
}

extern "C" char pde;

int vmap_frame_kernel(uint8_t*vaddr, uint8_t*paddr)
{
    printk("mapping %p -> %p\n", vaddr, paddr);
    auto ppde = &pde;
    auto pml4_offset = ((uint64_t)vaddr & 0xff8000000000) >> 39;
    auto pdpe_offset = ((uint64_t)vaddr & 0x7fc0000000) >> 30;
    auto pde_offset = ((uint64_t)vaddr & 0x3fe00000) >> 21;
    auto pte_offset = ((uint64_t)vaddr & 0x1ff000) >> 12;

    Page_PDE *pde = (Page_PDE *)Phy_To_Virt(ppde);

    Page_PTE *pte = (Page_PTE *)Phy_To_Virt(pde[pde_offset].NEXT << PAGE_4K_SHIFT);
    bool swap_used = false;
    if ((uint64_t)pte == PAGE_OFFSET)
    {
        pde[pde_offset].NEXT = swap_pte.paddr >> PAGE_4K_SHIFT;
        *(uint64_t *)&pde[pde_offset] |= 0x3;
        pte = (Page_PTE *)swap_pte.vaddr;
        swap_used = true;
    }

    // if (pte[pte_offset].P == 1)
    // {
    //     return -1;
    // }

    if (swap_used)
    {
        pte[pte_offset].PPBA = ((uint64_t)paddr) >> PAGE_4K_SHIFT;
        *(uint64_t *)&pte[pte_offset] |= 0x3;

        swap_pte.vaddr = (uint64_t)pte_alloc();
        swap_pte.paddr = (uint64_t)Virt_To_Phy(swap_pte.vaddr);
    }
    else
    {
        pte[pte_offset].PPBA = ((uint64_t)paddr) >> PAGE_4K_SHIFT;
        *(uint64_t *)&pte[pte_offset] |= 0x3;
    }

    flush_tlb();
    return 0;
}

// overload
int vmap_frame_kernel(uint64_t vaddr, uint64_t paddr)
{
    return vmap_frame_kernel((uint8_t*)vaddr, (uint8_t*)paddr);
}

int vmap_frame_kernel(uint8_t*vaddr)
{

    auto ppde = &pde;
    auto pde_offset = ((uint64_t)vaddr & 0x3fe00000) >> 21;
    auto pte_offset = ((uint64_t)vaddr & 0x1ff000) >> 12;

    Page_PDE *pde = (Page_PDE *)Phy_To_Virt(ppde);

    Page_PTE *pte = (Page_PTE *)Phy_To_Virt(pde[pde_offset].NEXT << PAGE_4K_SHIFT);
    bool swap_used = false;
    if ((uint64_t)pte == PAGE_OFFSET)
    {
        pde[pde_offset].NEXT = swap_pte.paddr >> PAGE_4K_SHIFT;
        *(uint64_t *)&pde[pde_offset] |= 0x3;
        pte = (Page_PTE *)swap_pte.vaddr;
        swap_used = true;
    }

    if (pte[pte_offset].P == 1)
    {
        return -1;
    }

    if (swap_used)
    {

        auto page = PhysicalMemory::GetInstance()->Allocate(1, 0);
        pte[pte_offset].PPBA = ((uint64_t)page->physical_address) >> PAGE_4K_SHIFT;
        *(uint64_t *)&pte[pte_offset] |= 0x3;

        swap_pte.vaddr = (uint64_t)pte_alloc();
        swap_pte.paddr = (uint64_t)Virt_To_Phy(swap_pte.vaddr);

        flush_tlb();
    }
    else
    {
        auto page = PhysicalMemory::GetInstance()->Allocate(1, 0);
        pte[pte_offset].PPBA = ((uint64_t)page->physical_address) >> PAGE_4K_SHIFT;
        *(uint64_t *)&pte[pte_offset] |= 0x3;
    }

    return 0;
}

int vmap_frame(task_struct *task, uint64_t vstart, uint64_t attributes)
{
    if (!task->mm)
        return vmap_frame_kernel((uint8_t*)vstart);

    auto pml4_offset = (vstart & 0xff8000000000) >> 39;
    auto pdpe_offset = (vstart & 0x7fc0000000) >> 30;
    auto pde_offset = (vstart & 0x3fe00000) >> 21;
    auto pte_offset = (vstart & 0x1ff000) >> 12;

    auto pm_instance = PhysicalMemory::GetInstance();

    Page_PML4 *&pml4_entry = task->mm->pml4;

    if (pml4_entry == nullptr)
    {
        auto slot = pm_instance->Allocate(1, PG_PTable_Maped | PG_Active);
        list_init(&slot->list);
        task->mm->physical_page_list = slot->list;
        pml4_entry = (Page_PML4 *)Phy_To_Virt(slot->physical_address);
        bzero(pml4_entry, 0x1000);
    }

    Page_PDPE *pdpe = (Page_PDPE *)Phy_To_Virt(pml4_entry[pml4_offset].PDPE << PAGE_4K_SHIFT);
    if ((uint64_t)pdpe == PAGE_OFFSET)
    {
        auto slot = pm_instance->Allocate(1, PG_PTable_Maped | PG_Active);
        bzero(Phy_To_Virt(slot->physical_address), 0x1000);
        list_add_to_behind(&task->mm->physical_page_list, &slot->list);
        pml4_entry[pml4_offset].PDPE = (uint64_t)slot->physical_address >> PAGE_4K_SHIFT;
        *(uint64_t *)&pml4_entry[pml4_offset] |= attributes;
        pdpe = (Page_PDPE *)Phy_To_Virt(slot->physical_address);
    }

    Page_PDE *pde = (Page_PDE *)Phy_To_Virt(pdpe[pdpe_offset].NEXT << PAGE_4K_SHIFT);
    if ((uint64_t)pde == PAGE_OFFSET)
    {
        auto slot = pm_instance->Allocate(1, PG_PTable_Maped | PG_Active);
        bzero(Phy_To_Virt(slot->physical_address), 0x1000);
        list_add_to_behind(&task->mm->physical_page_list, &slot->list);
        pdpe[pdpe_offset].NEXT = (uint64_t)slot->physical_address >> PAGE_4K_SHIFT;
        *(uint64_t *)&pdpe[pdpe_offset] |= attributes;
        pde = (Page_PDE *)Phy_To_Virt(slot->physical_address);
    }

    Page_PTE *pte = (Page_PTE *)Phy_To_Virt(pde[pde_offset].NEXT << PAGE_4K_SHIFT);
    if ((uint64_t)pte == PAGE_OFFSET)
    {
        auto slot = pm_instance->Allocate(1, PG_PTable_Maped | PG_Active);
        bzero(Phy_To_Virt(slot->physical_address), 0x1000);
        list_add_to_behind(&task->mm->physical_page_list, &slot->list);
        pde[pde_offset].NEXT = (uint64_t)slot->physical_address >> PAGE_4K_SHIFT;
        *(uint64_t *)&pde[pde_offset] |= attributes;
        pte = (Page_PTE *)Phy_To_Virt(slot->physical_address);
    }

    if (pte[pte_offset].P == 1)
    {
        return -1;
    }

    auto max_vmap_count = 511 - pte_offset;
    max_vmap_count = max_vmap_count > 16 ? 16 : max_vmap_count;
    auto slot = pm_instance->Allocate(max_vmap_count, PG_PTable_Maped | PG_Active);
    bzero(Phy_To_Virt(slot->physical_address), 0x1000 * max_vmap_count);
    list_add_to_behind(&task->mm->physical_page_list, &slot->list);
    for (uint64_t i = 0; i <= max_vmap_count; ++i)
    {
        pte[pte_offset + i].PPBA = ((uint64_t)slot->physical_address + i * 0x1000) >> PAGE_4K_SHIFT;
        *(uint64_t *)&pte[pte_offset + i] |= attributes;
    }

    return 0;
}
