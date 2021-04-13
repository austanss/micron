#include "memory/memory.h"
#include "cpu/interrupts.h"
#include "userio/tty.h"
#include "devio/io.h"
#include "util/kutil.h"
#include "userio/kbd.h"
#include "devio/power.h"
#include "userio/serialcon.h"
#include "chrono/timer.h"
#include "config/kconf.h"

extern "C"
{

cpu::idt::idt_descriptor  	 idt_s[256];
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
extern void lidt(address);

typedef struct s_registers
{
	qword cr4;
	qword cr3;
	qword cr2;
	qword cr0;
	qword ds;
	qword rdi;
	qword rsi;
	qword rbp;
	qword rbx;
	qword rdx;
	qword rcx;
	qword rax;
	qword interrupt_number;
	qword error_code;
	qword rip;
	qword cs;
	qword rflags;
	qword rsp;
} registers;

const char* exception_messages[32] {
	"Division by Zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"x87 Floating-Point",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point",
	"Virtualization",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Security",
	"Reserved"
};

enum page_fault_bit {
	present,
	write,
	user,
	reserved_write,
	no_execute
};

char* $get_symbol_label(uint64 addr)
{
	for (int i = 0; sys::config::__kernel_symtab[i].addr != 0xFFFFFFFFFFFFFFFF; i++)

		if (addr >= sys::config::__kernel_symtab[i].addr 
			&& 
			addr < sys::config::__kernel_symtab[i + 1].addr
			&& 
			addr < (uint64)&sys::config::__kernel_end)

			return sys::config::__kernel_symtab[i].name;

	return (char *)"<unidentified function>";
}

void exception_handler(registers& regs)
{
	io::tty::clear();

	printf("\n\tFATAL ERROR: CPU Exception: %s\n\n", exception_messages[regs.interrupt_number]);
	printf("\tERROR CODE: %x\n\n\n", regs.error_code);

	if (regs.interrupt_number == 0xE)
	{
		printf("\t\t >>> Page Fault accessing %x\n", regs.cr2);

		printf("\t\t\t >> accessed %s page\n", ((regs.error_code >> page_fault_bit::present) & 1) ? "protected" : "non-present");
		
		if ((regs.error_code >> page_fault_bit::no_execute) & 1)
			printf("\t\t\t >> executed non-executable page\n");
			
		printf("\t\t\t >> during %s\n", ((regs.error_code >> page_fault_bit::write) & 1) ? "write" : "read");
		printf("\t\t\t >> inside %s\n", ((regs.error_code >> page_fault_bit::user) & 1) ? "userspace" : "kernel");

	}

	uint64 kaslr_slide = ((uint64)&sys::config::__kernel_start) - 0x200000;

	printf("\n\t\t >>> [de-KASLR'd] Instruction Pointer\n");
	printf("\t\t\t >> %x \n", regs.rip - kaslr_slide);

	printf("\n\t\t >>> Stacktrace\n");
	
	printf("\t\t\t >> %s\n", $get_symbol_label(regs.rip - kaslr_slide));

	uint64 rbp = regs.rbp;
	while (true)
	{
		if (!rbp)
			break;

		printf("\t\t\t >> %s\n", $get_symbol_label((*((uint64 *)(rbp + 8))) - kaslr_slide));
		rbp = *((uint64 *)rbp);
	}

	asm volatile("cli; hlt");
	cpu::power::restart_cold();
}

void isr_handler(registers& regs)
{
    exception_handler(regs);
}

uint32 milliseconds = 0;
uint64 seconds = 0;

extern void kbd_irq_handler();

void irq_handler(registers& regs)
{
	if (regs.interrupt_number == 32) // PIT
	{
		sys::chrono::ms_clock++;
		milliseconds++;
		if (milliseconds >= io::pit::frequency_hz)
		{
			milliseconds = 0;
			seconds++;
		}
	}

	if (regs.interrupt_number == 33) // keyboard
		kbd_irq_handler();

	if (regs.interrupt_number == 36) // com port
		io::serial::console::read_character();

    if (regs.interrupt_number >= 40)
        io::outb(0xA0, 0x20); /* slave */
    io::outb(0x20, 0x20); /* master */
}

void idt_set(uint08 number, address base, word selector, byte flags) {
	/* Set Base Address */
	idt_s[number].baseLow = base & 0xFFFF;
	idt_s[number].baseMid = (base >> 16) & 0xFFFF;
	idt_s[number].baseHigh = (base >> 32) & 0xFFFFFFFF;

	/* Set Selector */
	idt_s[number].selector = selector;
	idt_s[number].flags = flags;


	/* Set IST */
	if (number < 32)
		idt_s[number].reservedIst = 1;
	else if (number < 48)
		idt_s[number].reservedIst = 2;	
	else
		idt_s[number].reservedIst = 3;

	/* Set Reserved Areas to Zero */
	idt_s[number].reserved = 0;
}

void cpu::idt::load_idt()
{
	idtPointer.limit = sizeof(cpu::idt::idt_descriptor) * 256 - 1;
	idtPointer.base  = (address)&idt_s;

	memory::operations::memset((byte *)&idt_s, 0, sizeof(cpu::idt::idt_descriptor) * 256);

	idt_set(0,  (address)isr0,  0x08, 0x8E);
	idt_set(1,  (address)isr1,  0x08, 0x8E);
	idt_set(2,  (address)isr2,  0x08, 0x8E);
	idt_set(3,  (address)isr3,  0x08, 0x8E);
	idt_set(4,  (address)isr4,  0x08, 0x8E);
	idt_set(5,  (address)isr5,  0x08, 0x8E);
	idt_set(6,  (address)isr6,  0x08, 0x8E);
	idt_set(7,  (address)isr7,  0x08, 0x8E);
	idt_set(8,  (address)isr8,  0x08, 0x8E);
	idt_set(9,  (address)isr9,  0x08, 0x8E);
	idt_set(10, (address)isr10, 0x08, 0x8E);
	idt_set(11, (address)isr11, 0x08, 0x8E);
	idt_set(12, (address)isr12, 0x08, 0x8E);
	idt_set(13, (address)isr13, 0x08, 0x8E);
	idt_set(14, (address)isr14, 0x08, 0x8E);
	idt_set(15, (address)isr15, 0x08, 0x8E);
	idt_set(16, (address)isr16, 0x08, 0x8E);
	idt_set(17, (address)isr17, 0x08, 0x8E);
	idt_set(18, (address)isr18, 0x08, 0x8E);
	idt_set(19, (address)isr19, 0x08, 0x8E);
	idt_set(20, (address)isr20, 0x08, 0x8E);
	idt_set(21, (address)isr21, 0x08, 0x8E);
	idt_set(22, (address)isr22, 0x08, 0x8E);
	idt_set(23, (address)isr23, 0x08, 0x8E);
	idt_set(24, (address)isr24, 0x08, 0x8E);
	idt_set(25, (address)isr25, 0x08, 0x8E);
	idt_set(26, (address)isr26, 0x08, 0x8E);
	idt_set(27, (address)isr27, 0x08, 0x8E);
	idt_set(28, (address)isr28, 0x08, 0x8E);
	idt_set(29, (address)isr29, 0x08, 0x8E);
	idt_set(30, (address)isr30, 0x08, 0x8E);
	idt_set(31, (address)isr31, 0x08, 0x8E);
	idt_set(32, (address)irq0,  0x08, 0x8E);
	idt_set(33, (address)irq1,  0x08, 0x8E);
	idt_set(34, (address)irq2,  0x08, 0x8E);
	idt_set(35, (address)irq3,  0x08, 0x8E);
	idt_set(36, (address)irq4,  0x08, 0x8E);
	idt_set(37, (address)irq5,  0x08, 0x8E);
	idt_set(38, (address)irq6,  0x08, 0x8E);
	idt_set(39, (address)irq7,  0x08, 0x8E);
	idt_set(40, (address)irq8,  0x08, 0x8E);
	idt_set(41, (address)irq9,  0x08, 0x8E);
	idt_set(42, (address)irq10, 0x08, 0x8E);
	idt_set(43, (address)irq11, 0x08, 0x8E);
	idt_set(44, (address)irq12, 0x08, 0x8E);
	idt_set(45, (address)irq13, 0x08, 0x8E);
	idt_set(46, (address)irq14, 0x08, 0x8E);
	idt_set(47, (address)irq15, 0x08, 0x8E);

	lidt((address)&idtPointer);
}

}
