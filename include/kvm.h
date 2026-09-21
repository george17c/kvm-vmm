#ifndef __KVM_H__
#define __KVM_H__

#include <linux/kvm.h>
#include <stdio.h>
#include <stdint.h>

struct vm {
    int fd, kvm_fd;
    size_t mem_sz;
    void *mem_start;
    struct vcpu *vcpu;
};

int kvm_init();
struct vm *kvm_create_vm(uint64_t mem_sz, uint64_t guest_base);
void kvm_destroy_vm(struct vm *vm);

#endif /* __KVM_H__ */
