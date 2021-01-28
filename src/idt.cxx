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

cpu::idt::idt_descriptor   idt[256];
cpu::idt::idt_ptr			 idtPointer;

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
} registers;

void exception_handler(registers& registers)
{
	terminal::instance().clear();

	puts((char *)"$RED!FATAL ERROR: $WHITE!The CPU has thrown a fatal exception and the system must terminate.\n");
	puts((char *)"A register dump has been printed to serial output.\n");
	puts((char *)"\n");
	puts((char *)"The system will restart momentarily...");
	io::serial::serial_msg("\n\nFATAL ERROR: CPU EXCEPTION ");
	io::serial::serial_msg(util::itoa(registers.interruptNumber, 16));
	io::serial::serial_msg(" -/- ERROR CODE ");
	io::serial::serial_msg(util::itoa(registers.errorCode, 16));
	io::serial::serial_msg("\n");
	io::serial::serial_msg("DS: ");
	io::serial::serial_msg(util::itoa(registers.ds, 16));
	io::serial::serial_msg("\n");
	io::serial::serial_msg("RDI: ");
	io::serial::serial_msg(util::itoa(registers.rdi, 16));
	io::serial::serial_msg(" | RSI: ");
	io::serial::serial_msg(util::itoa(registers.rsi, 16));
	io::serial::serial_msg(" | RBP: ");
	io::serial::serial_msg(util::itoa(registers.rbp, 16));
	io::serial::serial_msg(" | RSP: ");
	io::serial::serial_msg(util::itoa(registers.ss, 16));
	io::serial::serial_msg("\n");
	io::serial::serial_msg("RBX: ");
	io::serial::serial_msg(util::itoa(registers.rbx, 16));
	io::serial::serial_msg(" | RDX: ");
	io::serial::serial_msg(util::itoa(registers.rdx, 16));
	io::serial::serial_msg(" | RCX: ");
	io::serial::serial_msg(util::itoa(registers.rcx, 16));
	io::serial::serial_msg(" | RAX: ");
	io::serial::serial_msg(util::itoa(registers.rax, 16));
	io::serial::serial_msg("\n");
	io::serial::serial_msg("RIP: ");
	io::serial::serial_msg(util::itoa(registers.rip, 16));
	io::serial::serial_msg(" | CS: ");
	io::serial::serial_msg(util::itoa(registers.cs, 16));
	io::serial::serial_msg("\n");
	io::serial::serial_msg("RFLAGS: ");
	io::serial::serial_msg(util::itoa(registers.rflags, 16));
	io::serial::serial_msg("\n");
	io::serial::serial_msg("\n");
	asm volatile("cli");
	cpu::power::restart_cold();
}

void isr_handler(registers& registers)
{
    exception_handler(registers);
}

void irq_handler(registers& registers)
{
	if (registers.interruptNumber == 33)
	{
		uint8_t keycode = io::inb(0x60);;

		char* char_buffer = (char *)"$WHITE! \0";
		char_buffer[7] = io::keyboard::scan_code_to_char(keycode);

		terminal::instance().write(char_buffer);

		prevKeyCode = keycode;
	}

    if (registers.interruptNumber >= 40)
        io::outb(0xA0, 0x20); /* slave */
    io::outb(0x20, 0x20); /* master */
}

void idt_set(uint8_t number, uint64_t base, uint16_t selector, uint8_t flags) {
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

void load_idt()
{
	idtPointer.limit = sizeof(cpu::idt::idt_descriptor) * 256 - 1;
	idtPointer.base  = (uint64_t)&idt;

	memory::operations::memset((uint8_t *)&idt, 0, sizeof(cpu::idt::idt_descriptor) * 256);

	idt_set(0,  (uint64_t)isr0,  0x08, 0x8E);
	idt_set(1,  (uint64_t)isr1,  0x08, 0x8E);
	idt_set(2,  (uint64_t)isr2,  0x08, 0x8E);
	idt_set(3,  (uint64_t)isr3,  0x08, 0x8E);
	idt_set(4,  (uint64_t)isr4,  0x08, 0x8E);
	idt_set(5,  (uint64_t)isr5,  0x08, 0x8E);
	idt_set(6,  (uint64_t)isr6,  0x08, 0x8E);
	idt_set(7,  (uint64_t)isr7,  0x08, 0x8E);
	idt_set(8,  (uint64_t)isr8,  0x08, 0x8E);
	idt_set(9,  (uint64_t)isr9,  0x08, 0x8E);
	idt_set(10, (uint64_t)isr10, 0x08, 0x8E);
	idt_set(11, (uint64_t)isr11, 0x08, 0x8E);
	idt_set(12, (uint64_t)isr12, 0x08, 0x8E);
	idt_set(13, (uint64_t)isr13, 0x08, 0x8E);
	idt_set(14, (uint64_t)isr14, 0x08, 0x8E);
	idt_set(15, (uint64_t)isr15, 0x08, 0x8E);
	idt_set(16, (uint64_t)isr16, 0x08, 0x8E);
	idt_set(17, (uint64_t)isr17, 0x08, 0x8E);
	idt_set(18, (uint64_t)isr18, 0x08, 0x8E);
	idt_set(19, (uint64_t)isr19, 0x08, 0x8E);
	idt_set(20, (uint64_t)isr20, 0x08, 0x8E);
	idt_set(21, (uint64_t)isr21, 0x08, 0x8E);
	idt_set(22, (uint64_t)isr22, 0x08, 0x8E);
	idt_set(23, (uint64_t)isr23, 0x08, 0x8E);
	idt_set(24, (uint64_t)isr24, 0x08, 0x8E);
	idt_set(25, (uint64_t)isr25, 0x08, 0x8E);
	idt_set(26, (uint64_t)isr26, 0x08, 0x8E);
	idt_set(27, (uint64_t)isr27, 0x08, 0x8E);
	idt_set(28, (uint64_t)isr28, 0x08, 0x8E);
	idt_set(29, (uint64_t)isr29, 0x08, 0x8E);
	idt_set(30, (uint64_t)isr30, 0x08, 0x8E);
	idt_set(31, (uint64_t)isr31, 0x08, 0x8E);
	idt_set(32, (uint64_t)irq0,  0x08, 0x8E);
	idt_set(33, (uint64_t)irq1,  0x08, 0x8E);
	idt_set(34, (uint64_t)irq2,  0x08, 0x8E);
	idt_set(35, (uint64_t)irq3,  0x08, 0x8E);
	idt_set(36, (uint64_t)irq4,  0x08, 0x8E);
	idt_set(37, (uint64_t)irq5,  0x08, 0x8E);
	idt_set(38, (uint64_t)irq6,  0x08, 0x8E);
	idt_set(39, (uint64_t)irq7,  0x08, 0x8E);
	idt_set(40, (uint64_t)irq8,  0x08, 0x8E);
	idt_set(41, (uint64_t)irq9,  0x08, 0x8E);
	idt_set(42, (uint64_t)irq10, 0x08, 0x8E);
	idt_set(43, (uint64_t)irq11, 0x08, 0x8E);
	idt_set(44, (uint64_t)irq12, 0x08, 0x8E);
	idt_set(45, (uint64_t)irq13, 0x08, 0x8E);
	idt_set(46, (uint64_t)irq14, 0x08, 0x8E);
	idt_set(47, (uint64_t)irq15, 0x08, 0x8E);

	lidt((uint64_t)&idtPointer);
}

}
