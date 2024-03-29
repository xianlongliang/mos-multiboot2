#pragma once

#include <thread/task.h>

// vstart must aligned to 4K
void vmap_init();
int vmap_frame(task_struct *task, uint64_t vstart, uint64_t attributes);

// alloc physical page manually
int vmap_frame_kernel(uint8_t*vaddr, uint8_t*paddr);
int vmap_frame_kernel(uint64_t vaddr, uint64_t paddr);

// alloc physical page automatically
int vmap_frame_kernel(uint8_t*vaddr);
