

.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr22
.global isr23
.global isr24
.global isr25
.global isr26
.global isr27
.global isr28
.global isr29
.global isr30
.global isr31
#  0: Divide By Zero Exception
isr0:
    cli
    push $0    # A normal ISR stub that pops a dummy error code to keep a
                   # uniform stack frame
    push $0
    jmp isr_common_stub

#  1: Debug Exception
isr1:
    cli
    push  $0
    push  $1
    jmp isr_common_stub

#  2: Non Maskable Interrupt Exception
isr2:
    cli
    push  $0
    push  $2
    jmp isr_common_stub

#  3:  	Breakpoint Exception
isr3:
    cli
    push  $0
    push  $3
    jmp isr_common_stub

#  : Into Detected Overflow Exception
isr4:
    cli
    push  $0
    push  $4
    jmp isr_common_stub

#  5: Out of Bounds Exception
isr5:
    cli
    push  $0
    push  $5
    jmp isr_common_stub

#  6:  	Invalid Opcode Exception
isr6:
    cli
    push  $0
    push  $6
    jmp isr_common_stub

#  7: No Coprocessor Exception
isr7:
    cli
    push  $0
    push  $7
    jmp isr_common_stub

#  8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push  $8        # Note that we DON'T push a value on the stack in this one!
                   # It pushes one already! Use this type of stub for exceptions
                   # that pop error codes!
    jmp isr_common_stub

#  9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push  $0
    push  $9
    jmp isr_common_stub

#  10: Bad TSS Exception
isr10:
    cli
    push  $10        # Note that we DON'T push a value on the stack in this one!
                   # It pushes one already! Use this type of stub for exceptions
                   # that pop error codes!
    jmp isr_common_stub

#  11: Segment Not Present Exception
isr11:
    cli
    push  $11       # Note that we DON'T push a value on the stack in this one!
                   # It pushes one already! Use this type of stub for exceptions
                   # that pop error codes!
    jmp isr_common_stub

#  12: Stack Fault Exception
isr12:
    cli
    push  $12       # Note that we DON'T push a value on the stack in this one!
                   # It pushes one already! Use this type of stub for exceptions
                   # that pop error codes!
    jmp isr_common_stub

#  13: General Protection Fault Exception
isr13:
    cli
    push  $13       # Note that we DON'T push a value on the stack in this one!
                   # It pushes one already! Use this type of stub for exceptions
                   # that pop error codes!
    jmp isr_common_stub

#  14: Page Fault Exception
isr14:
    cli
    push  $14       # Note that we DON'T push a value on the stack in this one!
                   # It pushes one already! Use this type of stub for exceptions
                   # that pop error codes!
    jmp isr_common_stub

#  15: Unknown Interrupt Exception
isr15:
    cli
    push  $0
    push  $15
    jmp isr_common_stub

#  16: Coprocessor Fault Exception
isr16:
    cli
    push  $0
    push  $16
    jmp isr_common_stub

#  17: Alignment Check Exception (486+)
isr17:
    cli
    push  $0
    push  $17
    jmp isr_common_stub

#  18: Machine Check Exception (Pentium/586+)
isr18:
    cli
    push  $0
    push  $18
    jmp isr_common_stub

#  19: Reserved Exception 19
isr19:
    cli
    push  $0
    push  $19
    jmp isr_common_stub

#  20: Reserved Exception 20
isr20:
    cli
    push  $0
    push  $20
    jmp isr_common_stub

#  21: Reserved Exception 21
isr21:
    cli
    push  $0
    push  $21
    jmp isr_common_stub

#  22: Reserved Exception 22
isr22:
    cli
    push  $0
    push  $22
    jmp isr_common_stub

#  23: Reserved Exception 23
isr23:
    cli
    push  $0
    push  $23
    jmp isr_common_stub

#  24: Reserved Exception 24
isr24:
    cli
    push  $0
    push  $24
    jmp isr_common_stub

#  25: Reserved Exception 25
isr25:
    cli
    push  $0
    push  $25
    jmp isr_common_stub

#  26: Reserved Exception 26
isr26:
    cli
    push  $0
    push  $26
    jmp isr_common_stub

#  27: Reserved Exception 27
isr27:
    cli
    push  $0
    push  $27
    jmp isr_common_stub

#  28: Reserved Exception 28
isr28:
    cli
    push  $0
    push  $28
    jmp isr_common_stub

#  29: Reserved Exception 29
isr29:
    cli
    push  $0
    push  $29
    jmp isr_common_stub

#  30: Reserved Exception 30
isr30:
    cli
    push  $0
    push  $30
    jmp isr_common_stub

#  31: Reserved Exception 31
isr31:
    cli
    push  $0
    push  $31
    jmp isr_common_stub

.extern fault_handler

.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15

# 32: IRQ0
irq0:
    cli
    push $0
    push $32
    jmp irq_common_stub

# 33: IRQ1
irq1:
    cli
    push $0
    push $33
    jmp irq_common_stub

# 34: IRQ2
irq2:
    cli
    push $0
    push $34
    jmp irq_common_stub

# 35: IRQ3
irq3:
    cli
    push $0
    push $35
    jmp irq_common_stub

# 36: IRQ4
irq4:
    cli
    push $0
    push $36
    jmp irq_common_stub

# 37: IRQ5
irq5:
    cli
    push $0
    push $37
    jmp irq_common_stub

# 38: IRQ6
irq6:
    cli
    push $0
    push $38
    jmp irq_common_stub

# 39: IRQ7
irq7:
    cli
    push $0
    push $39
    jmp irq_common_stub

# 40: IRQ8
irq8:
    cli
    push $0
    push $40
    jmp irq_common_stub

# 41: IRQ9
irq9:
    cli
    push $0
    push $41
    jmp irq_common_stub

# 42: IRQ10
irq10:
    cli
    push $0
    push $42
    jmp irq_common_stub

# 43: IRQ11
irq11:
    cli
    push $0
    push $43
    jmp irq_common_stub

# 44: IRQ12
irq12:
    cli
    push $0
    push $44
    jmp irq_common_stub

# 45: IRQ13
irq13:
    cli
    push $0
    push $45
    jmp irq_common_stub

# 46: IRQ14
irq14:
    cli
    push $0
    push $46
    jmp irq_common_stub

# 47: IRQ15
irq15:
    cli
    push $0
    push $47
    jmp irq_common_stub

.extern irq_handler

irq_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10,%ax
    mov %ax,%ds
    mov %ax,%es
    mov %ax,%fs
    mov %ax,%gs
    mov %esp,%eax
    push %eax
    mov $irq_handler,%eax
    call *%eax
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8,%esp
    iret


isr_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10,%ax
    mov %ax,%ds
    mov %ax,%es
    mov %ax,%fs
    mov %ax,%gs
    mov %esp,%eax
    push %eax
    mov $fault_handler,%eax
    call *%eax    # A special call, preserves the 'eip' register
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8,%esp # Cleans up the pushed error code and pushed ISR number
    iret       # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
