#include "descriptor.h"
#include <std/printk.h>
#include <std/kstring.h>
#include <memory/physical.h>

atomic<DescriptorNode> AvailDesc;

Descriptor *DescAlloc()
{
    printk("%d\n", sizeof(Descriptor));

    DescriptorNode oldHead = AvailDesc.load();
    while (true)
    {
        Descriptor *desc = oldHead.GetDesc();
        if (desc)
        {
            DescriptorNode newHead = desc->nextFree.load();
            newHead.Set(newHead.GetDesc(), oldHead.GetCounter());
            if (AvailDesc.compare_exchange(oldHead, newHead))
            {
                return desc;
            }
        }
        else
        {
            // allocate several pages
            // get first descriptor, this is returned to caller
            // alloc 16 * 4096 bytes
            static auto pm = PhysicalMemory::GetInstance();
            auto page = pm->Allocate(16, 0);
            char *ptr = (char *)Phy_To_Virt(page->physical_address);
            bzero(ptr, DESCRIPTOR_BLOCK_SZ);
            Descriptor *ret = (Descriptor *)ptr;
            // organize list with the rest of descriptors
            // and add to available descriptors
            {
                Descriptor *first = nullptr;
                Descriptor *prev = nullptr;

                char *currPtr = ptr + sizeof(Descriptor);
                currPtr = ALIGN_ADDR(currPtr, CACHELINE);
                first = (Descriptor *)currPtr;
                auto desc_end = ptr + DESCRIPTOR_BLOCK_SZ;
                while (currPtr + sizeof(Descriptor) < desc_end)
                {
                    Descriptor *curr = (Descriptor *)currPtr;
                    if (prev)
                        prev->nextFree.store(curr);

                    prev = curr;
                    currPtr = currPtr + sizeof(Descriptor);
                    currPtr = ALIGN_ADDR(currPtr, CACHELINE);
                }

                prev->nextFree.store({nullptr});

                // add list to available descriptors
                DescriptorNode oldHead = AvailDesc.load();
                DescriptorNode newHead;
                do
                {
                    prev->nextFree.store(oldHead);
                    newHead.Set(first, oldHead.GetCounter() + 1);
                } while (!AvailDesc.compare_exchange(oldHead, newHead));
            }

            return ret;
        }
    }
}