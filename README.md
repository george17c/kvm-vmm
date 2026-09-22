# kvm-vmm

This project is a minimal, educational Virtual Machine Monitor (VMM) written in C using the Linux KVM API. 

Right now, the project is in its early stages. It allocates guest RAM and executes a hardcoded 16-bit assembly snippet. While the command-line interface requires a file path and checks that the file exists, the program does not parse or load ELF kernels _yet_. Instead, it copies test instructions into memory, handles simple serial output on port `0x3f8`, and stops on `HLT`.

### Build

You need a Linux machine with KVM enabled and access to `/dev/kvm`.

Compile the binary with `make`:

```bash
make
```

### Usage

Run the executable with an assigned memory size and any dummy file path (existent):

```bash
./vmm -m 8M /tmp
```

The -m flag accepts memory values suffixed with K, M, or G. The allocator automatically aligns this value to the host system page size. On a successful run, the test program writes `4` to the terminal through the virtual serial port and halts.

### Notes

The hardcoded test bytecode is adapted from the [LWN.net](https://lwn.net/) article [Using the KVM API](https://lwn.net/Articles/658511/).