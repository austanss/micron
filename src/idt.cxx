#include "kernel/memory.hxx"
#include "kernel/idt.hxx"
#include "kernel/terminal.hxx"
#include "kernel/io.hxx"
#include "kernel/kutil.hxx"
#include "kernel/kbd.hxx"
#include "kernel/power.hxx"

uint8_t prevKeyCode = 156;

extern "C"
{

idt_descriptor   idt[256];
idt_ptr			 idtPointer;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void lidt(uint64_t);

typedef struct s_registers
{
	uint64_t ds;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t rbx;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rax;
	uint64_t interruptNumber;
	uint64_t errorCode;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t ss;
} Registers;

void ExceptionHandler(Registers& registers)
{
	Terminal& terminal = Terminal::instance();
	terminal.clear();

	puts("$RED!FATAL ERROR: $WHITE!The CPU has thrown a fatal exception and the system must terminate.\n");
	puts("A register dump has been printed to serial output.\n");
	puts("\n");
	puts("The system will restart momentarily...");
	serial_msg("\n\nFATAL ERROR: CPU EXCEPTION ");
	int2serial_hex(registers.interruptNumber);
	serial_msg(" -/- ERROR CODE ");
	int2serial_hex(registers.errorCode);
	serial_msg("\n");
	serial_msg("DS: ");
	int2serial_hex(registers.ds);
	serial_msg("\n");
	serial_msg("RDI: ");
	int2serial_hex(registers.rdi);
	serial_msg(" | RSI: ");
	int2serial_hex(registers.rsi);
	serial_msg(" | RBP: ");
	int2serial_hex(registers.rbp);
	serial_msg(" | RSP: ");
	int2serial_hex(registers.ss);
	serial_msg("\n");
	serial_msg("RBX: ");
	int2serial_hex(registers.rbx);
	serial_msg(" | RDX: ");
	int2serial_hex(registers.rdx);
	serial_msg(" | RCX: ");
	int2serial_hex(registers.rcx);
	serial_msg(" | RAX: ");
	int2serial_hex(registers.rax);
	serial_msg("\n");
	serial_msg("RIP: ");
	int2serial_hex(registers.rip);
	serial_msg(" | CS: ");
	int2serial_hex(registers.cs);
	serial_msg("\n");
	serial_msg("RFLAGS: ");
	int2serial_hex(registers.rflags);
	serial_msg("\n");
	serial_msg("\n");
	asm volatile("cli");
	restart_cold();
}

void ISRHandler(Registers& registers)
{
    ExceptionHandler(registers);
}

void IRQHandler(Registers& registers)
{
	if (registers.interruptNumber == 33)
	{
		uint8_t keycode = inb(0x60);;

		char* charr = "$WHITE! \0";
		charr[7] = getChar(keycode);

		Terminal::instance().write(charr);

		prevKeyCode = keycode;
	}

    if (registers.interruptNumber >= 40)
        outb(0xA0, 0x20); /* slave */
    outb(0x20, 0x20); /* master */
}

void idtSet(uint8_t number, uint64_t base, uint16_t selector, uint8_t flags) {
	/* Set Base Address */
	idt[number].baseLow = base & 0xFFFF;
	idt[number].baseMid = (base >> 16) & 0xFFFF;
	idt[number].baseHigh = (base >> 32) & 0xFFFFFFFF;

	/* Set Selector */
	idt[number].selector = selector;
	idt[number].flags = flags;

	/* Set Reserved Areas to Zero */
	idt[number].reservedIst = 0;
	idt[number].reserved = 0;
}

void loadIDT()
{
	idtPointer.limit = sizeof(idt_descriptor) * 256 - 1;
	idtPointer.base  = (uint64_t)&idt;

	memset((uint8_t *)&idt, 0, sizeof(idt_descriptor) * 256);

	idtSet(0,  (uint64_t)isr0,  0x08, 0x8E);
	idtSet(1,  (uint64_t)isr1,  0x08, 0x8E);
	idtSet(2,  (uint64_t)isr2,  0x08, 0x8E);
	idtSet(3,  (uint64_t)isr3,  0x08, 0x8E);
	idtSet(4,  (uint64_t)isr4,  0x08, 0x8E);
	idtSet(5,  (uint64_t)isr5,  0x08, 0x8E);
	idtSet(6,  (uint64_t)isr6,  0x08, 0x8E);
	idtSet(7,  (uint64_t)isr7,  0x08, 0x8E);
	idtSet(8,  (uint64_t)isr8,  0x08, 0x8E);
	idtSet(9,  (uint64_t)isr9,  0x08, 0x8E);
	idtSet(10, (uint64_t)isr10, 0x08, 0x8E);
	idtSet(11, (uint64_t)isr11, 0x08, 0x8E);
	idtSet(12, (uint64_t)isr12, 0x08, 0x8E);
	idtSet(13, (uint64_t)isr13, 0x08, 0x8E);
	idtSet(14, (uint64_t)isr14, 0x08, 0x8E);
	idtSet(15, (uint64_t)isr15, 0x08, 0x8E);
	idtSet(16, (uint64_t)isr16, 0x08, 0x8E);
	idtSet(17, (uint64_t)isr17, 0x08, 0x8E);
	idtSet(18, (uint64_t)isr18, 0x08, 0x8E);
	idtSet(19, (uint64_t)isr19, 0x08, 0x8E);
	idtSet(20, (uint64_t)isr20, 0x08, 0x8E);
	idtSet(21, (uint64_t)isr21, 0x08, 0x8E);
	idtSet(22, (uint64_t)isr22, 0x08, 0x8E);
	idtSet(23, (uint64_t)isr23, 0x08, 0x8E);
	idtSet(24, (uint64_t)isr24, 0x08, 0x8E);
	idtSet(25, (uint64_t)isr25, 0x08, 0x8E);
	idtSet(26, (uint64_t)isr26, 0x08, 0x8E);
	idtSet(27, (uint64_t)isr27, 0x08, 0x8E);
	idtSet(28, (uint64_t)isr28, 0x08, 0x8E);
	idtSet(29, (uint64_t)isr29, 0x08, 0x8E);
	idtSet(30, (uint64_t)isr30, 0x08, 0x8E);
	idtSet(31, (uint64_t)isr31, 0x08, 0x8E);
	idtSet(32, (uint64_t)irq0,  0x08, 0x8E);
	idtSet(33, (uint64_t)irq1,  0x08, 0x8E);
	idtSet(34, (uint64_t)irq2,  0x08, 0x8E);
	idtSet(35, (uint64_t)irq3,  0x08, 0x8E);
	idtSet(36, (uint64_t)irq4,  0x08, 0x8E);
	idtSet(37, (uint64_t)irq5,  0x08, 0x8E);
	idtSet(38, (uint64_t)irq6,  0x08, 0x8E);
	idtSet(39, (uint64_t)irq7,  0x08, 0x8E);
	idtSet(40, (uint64_t)irq8,  0x08, 0x8E);
	idtSet(41, (uint64_t)irq9,  0x08, 0x8E);
	idtSet(42, (uint64_t)irq10, 0x08, 0x8E);
	idtSet(43, (uint64_t)irq11, 0x08, 0x8E);
	idtSet(44, (uint64_t)irq12, 0x08, 0x8E);
	idtSet(45, (uint64_t)irq13, 0x08, 0x8E);
	idtSet(46, (uint64_t)irq14, 0x08, 0x8E);
	idtSet(47, (uint64_t)irq15, 0x08, 0x8E);

	lidt((uint64_t)&idtPointer);
}

}
