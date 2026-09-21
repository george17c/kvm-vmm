#include "kvm.h"
#include "mem.h"
#include "vcpu.h"
#include "assert.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

int kvm_init()
{
    int kvm, ret;

    kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);
    ASSERT(kvm > 0, "Cannot open /dev/kvm");

    ret = ioctl(kvm, KVM_GET_API_VERSION, NULL);
    ASSERT(ret != -1, "KVM_GET_API_VERSION");
    ASSERT(ret == 12, "KVM_GET_API_VERSION %d, expected 12", ret);

    ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
    ASSERT(ret != -1, "KVM_CHECK_EXTENSION");
    ASSERT(ret, "Required extension KVM_CAP_USER_MEM not available");

    return kvm;
}

struct vm *kvm_create_vm(uint64_t mem_sz, uint64_t entry)
{
    struct vm *vm;

    vm = malloc(sizeof(struct vm));
    ASSERT(vm != NULL, "kvm_create_vm");

    vm->kvm_fd = kvm_init();
    vm->fd = ioctl(vm->kvm_fd, KVM_CREATE_VM, (unsigned long)0);
    ASSERT(vm->fd > 0, "KVM_CREATE_VM");

    vm_set_memory_region(vm, mem_sz, entry);

    return vm;
}

void kvm_destroy_vm(struct vm *vm)
{
    close(vm->fd);
    close(vm->kvm_fd);
    close(vm->vcpu->fd);

    munmap(vm->mem_start, vm->mem_sz);
    munmap(vm->vcpu->run, vm->vcpu->run_sz);

    free(vm->vcpu);
    free(vm);
}
