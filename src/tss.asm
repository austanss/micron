global tss
global tss.kernel_rsp

tss:
    .reserved_0
    dd 0            ; reserved
    .kernel_rsp           
    dq 0            ; pointer to kernel stack (must set)
    .null_rsps
    times 2 dq 0    ; null RSPs
    .reserved_1
    dq 0            ; Reserved
    .null_ists
    times 7 dq 0    ; null ISTs
    .reserved_2
    dq 0            ; Reserved
    .reserved_3
    dw 0            ; Reserved
    .iopb
    dw 104          ; IOPB (TSS size)