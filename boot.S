// AArch64 mode

.section ".text.boot"

.global _boot

    .org 0x80000

_boot:
    // Shut off extra cores
    mrs     x5, mpidr_el1
    and     x5, x5, #3
    cbnz    x5, halt

    // set stack before our code
    ldr     x5, =_boot
    mov     sp, x5

    // clear bss
    ldr     x5, =__bss_start__
    ldr     w6, =__bss_size__
1:  cbz     w6, 2f
    str     xzr, [x5], #8
    sub     w6, w6, #1
    cbnz    w6, 1b

    // jump to C code, should not return
2:  bl      main
    // for failsafe, halt this core
halt:
    wfe
    b        halt
