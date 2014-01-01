; This is the kernel's entry point. We could either call main here,
; or we can use this to setup the stack or other nice stuff, like
; perhaps setting up the GDT and segments. Please note that interrupts
; are disabled at this point: More on interrupts later!
[BITS 32]
section .text
extern main
global start
;; start:
;;     mov esp, _sys_stack
;;     jmp stublet
; This part MUST be 4byte aligned, so we solve that issue using 'ALIGN 4'
	; Multiboot constants
MULTIBOOT_PAGE_ALIGN	equ 1<<0
MULTIBOOT_MEMORY_INFO	equ 1<<1
MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Multiboot header (needed to boot from GRUB)
ALIGN 4
multiboot_header:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

start:
    lgdt [trickgdt]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:higherhalf
    ;; mov esp, _sys_stack     ; This points the stack to our new stack area
    ;; jmp stublet

higherhalf:
    mov esp, _sys_stack
    call main
    jmp $

; This is an endless loop here. Make a note of this: Later on, we
; will insert an 'extern _main', followed by 'call _main', right
; before the 'jmp $'.
stublet:
    cli        ;; Disable interrupts before loading the gdt and idt
    ;; mov eax, cr0
    ;; and al, 0xfe          ;; Unset lowest cr0 bit
    ;; mov cr0, eax
    push ebx
    call main
    jmp $


global gdt_flush
extern gp
gdt_flush:
    lgdt [gp]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2
flush2:
    ret
global idt_load
extern idtp
idt_load:
    lidt[idtp]
    ret


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
;  0: Divide By Zero Exception
isr0:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

;  2: Non Maskable Interrupt Exception
isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

;  3:  	Breakpoint Exception
isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

;  : Into Detected Overflow Exception
isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

;  5: Out of Bounds Exception
isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

;  6:  	Invalid Opcode Exception
isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

;  7: No Coprocessor Exception
isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

;  8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push byte 8        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

;  9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

;  10: Bad TSS Exception
isr10:
    cli
    push byte 10        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

;  11: Segment Not Present Exception
isr11:
    cli
    push byte 11       ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

;  12: Stack Fault Exception
isr12:
    cli
    push byte 12       ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

;  13: General Protection Fault Exception
isr13:
    cli
    push byte 13       ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

;  14: Page Fault Exception
isr14:
    cli
    push byte 14       ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

;  15: Unknown Interrupt Exception
isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

;  16: Coprocessor Fault Exception
isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

;  17: Alignment Check Exception (486+)
isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

;  18: Machine Check Exception (Pentium/586+)
isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

;  19: Reserved Exception 19
isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

;  20: Reserved Exception 20
isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

;  21: Reserved Exception 21
isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

;  22: Reserved Exception 22
isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

;  23: Reserved Exception 23
isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

;  24: Reserved Exception 24
isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

;  25: Reserved Exception 25
isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

;  26: Reserved Exception 26
isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

;  27: Reserved Exception 27
isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

;  28: Reserved Exception 28
isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

;  29: Reserved Exception 29
isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

;  30: Reserved Exception 30
isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

;  31: Reserved Exception 31
isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub

extern fault_handler

isr_common_stub:
    pusha
    ;; push dr7
    ;; push dr6
    ;; push dr5
    ;; push dr4
    ;; push dr3
    ;; push dr2
    ;; push dr1
    ;; push dr0
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, fault_handler
    call eax    ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8 ; Cleans up the pushed error code and pushed ISR number
    iret       ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


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

; 32: IRQ0
irq0:
    cli
    push byte 0
    push byte 32
    jmp irq_common_stub

; 33: IRQ1
irq1:
    cli
    push byte 0
    push byte 33
    jmp irq_common_stub

; 34: IRQ2
irq2:
    cli
    push byte 0
    push byte 34
    jmp irq_common_stub

; 35: IRQ3
irq3:
    cli
    push byte 0
    push byte 35
    jmp irq_common_stub

; 36: IRQ4
irq4:
    cli
    push byte 0
    push byte 36
    jmp irq_common_stub

; 37: IRQ5
irq5:
    cli
    push byte 0
    push byte 37
    jmp irq_common_stub

; 38: IRQ6
irq6:
    cli
    push byte 0
    push byte 38
    jmp irq_common_stub

; 39: IRQ7
irq7:
    cli
    push byte 0
    push byte 39
    jmp irq_common_stub

; 40: IRQ8
irq8:
    cli
    push byte 0
    push byte 40
    jmp irq_common_stub

; 41: IRQ9
irq9:
    cli
    push byte 0
    push byte 41
    jmp irq_common_stub

; 42: IRQ10
irq10:
    cli
    push byte 0
    push byte 42
    jmp irq_common_stub

; 43: IRQ11
irq11:
    cli
    push byte 0
    push byte 43
    jmp irq_common_stub

; 44: IRQ12
irq12:
    cli
    push byte 0
    push byte 44
    jmp irq_common_stub

; 45: IRQ13
irq13:
    cli
    push byte 0
    push byte 45
    jmp irq_common_stub

; 46: IRQ14
irq14:
    cli
    push byte 0
    push byte 46
    jmp irq_common_stub

; 47: IRQ15
irq15:
    cli
    push byte 0
    push byte 47
    jmp irq_common_stub

extern irq_handler

irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, irq_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

[SECTION .setup]
trickgdt:
    dw gdt_end - gdt -1         ; size of GDT
    dd gdt                      ; linear address of GDT
gdt:
    dd 0, 0
    db 0xff, 0xff, 0, 0, 0, 10011010b, 11001111b, 0x40
    db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0x40
gdt_end:

; Here is the definition of our BSS section. Right now, we'll use
; it just to store the stack. Remember that a stack actually grows
; downwards, so we declare the size of the data before declaring
; the identifier '_sys_stack'
SECTION .bss
    resb 14336               ; This reserves 14KBytes of memory here
_sys_stack:
