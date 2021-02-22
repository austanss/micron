; Defined in idt.cxx
extern isr_handler
extern irq_handler

global lidt

default rel

lidt:
	lidt [rdi]
	ret

%macro pusha 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
%endmacro

%macro popa 0
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

; Common ISR code
isr_common_stub:
   ; Save CPU state into a structure,
   ; assembled onto the stack
   pusha 			; Pushes rdi, rsi, rbp and r[a-d]x
   mov ax, ds 		; Set the lower 16 bits of rax to ds
   push rax 		; save the value of rax, which is now ds
   mov ax, 0x10  	; kernel data segment descriptor
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   mov rax, cr0
   push rax ; gp control register
   mov rax, cr2
   push rax ; page fault faulty addy
   mov rax, cr3
   push rax ; paging table ptr
   mov rax, cr4
   push rax ; gp control register

   ; Since we assembled the struct
   ; on the stack, we can simply
   ; pass the stack pointer as a
   ; pointer to our structure
   mov rdi, rsp

  ; 2. Call C handler
   cld
   call isr_handler

  ; 3. Restore state
   pop rdx
   pop rdx
   pop rdx
   pop rdx
   pop rbx
   mov ds, bx
   mov es, bx
   mov fs, bx
   mov gs, bx
   popa
   add rsp, 16 ; Cleans up the pushed error code and pushed ISR number
   iretq ; pops 5 things at once: CS, RIP, RFLAGS, SS, and RSP

; Common IRQ code. Identical to ISR code except for the 'call'
; and the 'pop ebx'
irq_common_stub:
   ; Save CPU state into a structure,
   ; assembled onto the stack
   pusha 			; Pushes rdi, rsi, rbp and r[a-d]x
   mov ax, ds 		; Set the lower 16 bits of rax to ds
   push rax 		; save the value of rax, which is now ds
   mov ax, 0x10  	; kernel data segment descriptor
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   
   mov rax, cr0
   push rax ; gp control register
   mov rax, cr2
   push rax ; page fault faulty addy
   mov rax, cr3
   push rax ; page fault error info
   mov rax, cr4
   push rax ; gp control register

   ; Since we assembled the struct
   ; on the stack, we can simply
   ; pass the stack pointer as a
   ; pointer to our structure
   mov rdi, rsp

  ; 2. Call C handler
   cld
   lea r14, [rel irq_handler]
   call r14

  ; 3. Restore state
   pop rdx
   pop rdx
   pop rdx
   pop rdx
   pop rax
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   popa
   add rsp, 16 ; Cleans up the pushed error code and pushed ISR number
   iretq ; pops 5 things at once: CS, RIP, RFLAGS, SS, and RSP

; We don't get information about which interrupt was caller
; when the handler is run, so we will need to have a different handler
; for every interrupt.
; Furthermore, some interrupts push an error code onto the stack but others
; don't, so we will push a dummy error code for those which don't, so that
; we have a consistent stack for all of them.

; First make the ISRs global
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
; then IRQs

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; 0: Divide By Zero Exception
isr0:

    push 0
    push 0
    lea r14, [rel isr_common_stub]
    jmp r14

; 1: Debug Exception
isr1:

    push 0
    push 1
    lea r14, [rel isr_common_stub]
    jmp r14

; 2: Non Maskable Interrupt Exception
isr2:

    push 0
    push 2
    lea r14, [rel isr_common_stub]
    jmp r14

; 3: Int 3 Exception
isr3:
     
    push 0
    push 3
    lea r14, [rel isr_common_stub]
    jmp r14

; 4: INTO Exception
isr4:
     
    push 0
    push 4
    lea r14, [rel isr_common_stub]
    jmp r14

; 5: Out of Bounds Exception
isr5:
     
    push 0
    push 5
    lea r14, [rel isr_common_stub]
    jmp r14

; 6: Invalid Opcode Exception
isr6:
     
    push 0
    push 6
    lea r14, [rel isr_common_stub]
    jmp r14

; 7: Coprocessor Not Available Exception
isr7:
     
    push 0
    push 7
    lea r14, [rel isr_common_stub]
    jmp r14

; 8: Double Fault Exception (With Error Code!)
isr8:
     
    push 8
    lea r14, [rel isr_common_stub]
    jmp r14

; 9: Coprocessor Segment Overrun Exception
isr9:
     
    push 0
    push 9
    lea r14, [rel isr_common_stub]
    jmp r14

; 10: Bad TSS Exception (With Error Code!)
isr10:
     
    push 10
    lea r14, [rel isr_common_stub]
    jmp r14

; 11: Segment Not Present Exception (With Error Code!)
isr11:
     
    push 11
    lea r14, [rel isr_common_stub]
    jmp r14

; 12: Stack Fault Exception (With Error Code!)
isr12:
     
    push 12
    lea r14, [rel isr_common_stub]
    jmp r14

; 13: General Protection Fault Exception (With Error Code!)
isr13:
     
    push 13
    lea r14, [rel isr_common_stub]
    jmp r14

; 14: Page Fault Exception (With Error Code!)
isr14:
     
    push 14
    lea r14, [rel isr_common_stub]
    jmp r14

; 15: Reserved Exception
isr15:
     
    push 0
    push 15
    lea r14, [rel isr_common_stub]
    jmp r14

; 16: Floating Point Exception
isr16:
     
    push 0
    push 16
    lea r14, [rel isr_common_stub]
    jmp r14

; 17: Alignment Check Exception
isr17:
     
    push 0
    push 17
    lea r14, [rel isr_common_stub]
    jmp r14

; 18: Machine Check Exception
isr18:
     
    push 0
    push 18
    lea r14, [rel isr_common_stub]
    jmp r14

; 19: Reserved
isr19:
     
    push 0
    push 19
    lea r14, [rel isr_common_stub]
    jmp r14

; 20: Reserved
isr20:
     
    push 0
    push 20
    lea r14, [rel isr_common_stub]
    jmp r14

; 21: Reserved
isr21:
     
    push 0
    push 21
    lea r14, [rel isr_common_stub]
    jmp r14

; 22: Reserved
isr22:
     
    push 0
    push 22
    lea r14, [rel isr_common_stub]
    jmp r14

; 23: Reserved
isr23:
     
    push 0
    push 23
    lea r14, [rel isr_common_stub]
    jmp r14

; 24: Reserved
isr24:
     
    push 0
    push 24
    lea r14, [rel isr_common_stub]
    jmp r14

; 25: Reserved
isr25:
     
    push 0
    push 25
    lea r14, [rel isr_common_stub]
    jmp r14

; 26: Reserved
isr26:
     
    push 0
    push 26
    lea r14, [rel isr_common_stub]
    jmp r14

; 27: Reserved
isr27:
     
    push 0
    push 27
    lea r14, [rel isr_common_stub]
    jmp r14

; 28: Reserved
isr28:
     
    push 0
    push 28
    lea r14, [rel isr_common_stub]
    jmp r14

; 29: Reserved
isr29:
     
    push 0
    push 29
    lea r14, [rel isr_common_stub]
    jmp r14

; 30: Reserved
isr30:
     
    push 0
    push 30
    lea r14, [rel isr_common_stub]
    jmp r14

; 31: Reserved
isr31:
     
    push 0
    push 31
    lea r14, [rel isr_common_stub]
    jmp r14

; IRQ handlers

; This IRQ is the Programmable Interval Timer. Since it spams us with
; interrupts, we will ignore it until we have a use for chronology
irq0:	 
	push 0
	push 32
    lea r14, [rel irq_common_stub]
	jmp r14

irq1:
	 
	push 1
	push 33
	lea r14, [rel irq_common_stub]
	jmp r14

irq2:
	 
	push 2
	push 34
	lea r14, [rel irq_common_stub]
	jmp r14

irq3:
	 
	push 3
	push 35
	lea r14, [rel irq_common_stub]
	jmp r14

irq4:
	 
	push 4
	push 36
	lea r14, [rel irq_common_stub]
	jmp r14

irq5:
	 
	push 5
	push 37
	lea r14, [rel irq_common_stub]
	jmp r14

irq6:
	 
	push 6
	push 38
	lea r14, [rel irq_common_stub]
	jmp r14

irq7:
	 
	push 7
	push 39
	lea r14, [rel irq_common_stub]
	jmp r14

irq8:
	 
	push 8
	push 40
	lea r14, [rel irq_common_stub]
	jmp r14

irq9:
	 
	push 9
	push 41
	lea r14, [rel irq_common_stub]
	jmp r14

irq10:
	 
	push 10
	push 42
	lea r14, [rel irq_common_stub]
	jmp r14

irq11:
	 
	push 11
	push 43
	lea r14, [rel irq_common_stub]
	jmp r14

irq12:
	 
	push 12
	push 44
	lea r14, [rel irq_common_stub]
	jmp r14

irq13:
	 
	push 13
	push 45
	lea r14, [rel irq_common_stub]
	jmp r14

irq14:
	 
	push 14
	push 46
	lea r14, [rel irq_common_stub]
	jmp r14

irq15:
	 
	push 15
	push 47
	lea r14, [rel irq_common_stub]
	jmp r14
