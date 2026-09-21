#include "mem.h"
#include "kvm.h"
#include "assert.h"

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

void vm_set_memory_region(struct vm *vm, uint64_t mem_sz, uint64_t entry)
{
    int ret;

    size_t page_sz = (size_t)sysconf(_SC_PAGESIZE);
    ASSERT(page_sz > 0, "sysconf(_SC_PAGESIZE)");

    mem_sz = ALIGN_UP(mem_sz, page_sz);

    vm->mem_start = mmap(NULL, (size_t)mem_sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT(vm->mem_start != MAP_FAILED);
    vm->mem_sz = mem_sz;

    struct kvm_userspace_memory_region region = {
        .slot = 0,
        .guest_phys_addr = entry,
        .memory_size = mem_sz,
        .userspace_addr = (uint64_t)vm->mem_start,
    };

    ret = ioctl(vm->fd, KVM_SET_USER_MEMORY_REGION, &region);
    ASSERT(ret == 0, "KVM_SET_USER_MEMORY_REGION");
}
