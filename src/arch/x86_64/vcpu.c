#include "kvm.h"
#include "vcpu.h"
#include "assert.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <err.h>

void vcpu_reset_cs(int fd)
{
    struct kvm_sregs sregs;
    ioctl(fd, KVM_GET_SREGS, &sregs);
    sregs.cs.base = 0;
    sregs.cs.selector = 0;
    ioctl(fd, KVM_SET_SREGS, &sregs);
}

void vcpu_setup_example_program(int fd)
{
    struct kvm_regs regs = {
        .rip = 0x0000,
        .rax = 2,
        .rbx = 2,
        .rflags = 0x2,
    };
    ioctl(fd, KVM_SET_REGS, &regs);
}

void vcpu_create(struct vm *vm)
{
    struct vcpu *vcpu = malloc(sizeof(struct vcpu));
    ASSERT(vcpu != NULL, "vcpu_create");

    vcpu->fd = ioctl(vm->fd, KVM_CREATE_VCPU, (unsigned long)0);
    ASSERT(vcpu->fd > 0, "KVM_CREATE_VCPU");

    int mmap_size = ioctl(vm->kvm_fd, KVM_GET_VCPU_MMAP_SIZE, NULL);
    ASSERT(mmap_size > 0, "KVM_GET_VCPU_MMAP_SIZE");

    vcpu->run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu->fd, 0);
    vcpu->run_sz = mmap_size;

    vcpu_reset_cs(vcpu->fd);
    vcpu_setup_example_program(vcpu->fd);

    vm->vcpu = vcpu;
}

void vcpu_run(struct vm *vm)
{
    int ret;

    while (1) {
        ret = ioctl(vm->vcpu->fd, KVM_RUN, NULL);
        ASSERT(ret == 0, "KVM_RUN ioctl return code: %d", ret);

        switch (vm->vcpu->run->exit_reason) {
        case KVM_EXIT_HLT:
            puts("KVM_EXIT_HLT");
            return;
        case KVM_EXIT_IO:
            if (vm->vcpu->run->io.direction == KVM_EXIT_IO_OUT &&
                vm->vcpu->run->io.size == 1 &&
                vm->vcpu->run->io.port == 0x3f8 &&
                vm->vcpu->run->io.count == 1)
            putchar(*(((char *)vm->vcpu->run) + vm->vcpu->run->io.data_offset));
            else
            errx(1, "unhandled KVM_EXIT_IO");
            break;
        case KVM_EXIT_FAIL_ENTRY:
            errx(1, "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = 0x%llx",
             (unsigned long long)vm->vcpu->run->fail_entry.hardware_entry_failure_reason);
        case KVM_EXIT_INTERNAL_ERROR:
            errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
                 vm->vcpu->run->internal.suberror);
        }
    }
}