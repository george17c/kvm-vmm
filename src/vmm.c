#include "kvm.h"
#include "vcpu.h"
#include "assert.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

extern const uint8_t code[];
extern const int code_size;

void print_usage(char *program_name)
{
    printf("Usage: %s [args] FILE\n", program_name);
    printf("\nargs:\n");
    printf("\t-m string\tAssign memory to the VM (K, M, G). Default: 64KiB.\n");
    printf("\t-h\t\tPrint this menu and exit.\n");

    printf("\nFILE: path to the kernel image (ELF format)\n");

    printf("\nExample: %s -m 128K ./kernel\n", program_name);
}

static uint64_t parse_mem(const char *str)
{
    char *end;
    errno = 0;
    uint64_t val = strtoull(str, &end, 10);
    if (errno != 0 || end == str)
        return 0;

    if (!strcasecmp(end, "K"))
        return val << 10;
    if (!strcasecmp(end, "M"))
        return val << 20;
    if (!strcasecmp(end, "G"))
        return val << 30;

    return 0;
}

int main(int argc, char **argv)
{
    struct vm *vm;

    /* 64KiB default */
    uint64_t mem_sz = 64 << 10;
    int opt;

    while ((opt = getopt(argc, argv, "m:h")) != -1) {
        switch (opt) {
        case 'm':
            mem_sz = parse_mem(optarg);
            if (!mem_sz) {
                fprintf(stderr, "Error: Invalid memory size: %s\n", optarg);
                print_usage(argv[0]);
                return 1;
            }
            break;
        case 'h':
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: Missing kernel image\n");
        print_usage(argv[0]);
        return 1;
    }

    const char *kernel_path = argv[optind];

    int kernel_fd = open(kernel_path, O_RDONLY, 0);
    ASSERT(kernel_fd > 0, "kernel file '%s' does not exist", kernel_path);

    vm = kvm_create_vm(mem_sz, 0x0000);
    vcpu_create(vm);

    memcpy(vm->mem_start, code, code_size);

    vcpu_run(vm);

    kvm_destroy_vm(vm);

    return 0;
}
