#include "kernel/asmfuncs.hxx"
#include "kernel/idt.hxx"
#include "kernel/terminal.hxx"
#include "kernel/io.hxx"
#include "kernel/kutil.hxx"
#include "kernel/kbd.hxx"

uint8_t prevKeyCode = 156;

extern "C"
{

extern void flushIDT(uint64_t);

IDTEntry   idtEntries[256];
IDTPointer idtPointer;


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

struct Registers
{
	uint64_t ds;
	uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
	uint64_t interruptNumber, errorCode;
	uint64_t rip, cs, rflags, userrsp, ss;
};


void ExceptionHandler(Registers& registers)
{
	Terminal& terminal = Terminal::instance();

	terminal << "$RED!ERROR: $WHITE!" << registers.interruptNumber << " -/- ";
	writeHex(registers.errorCode);
	terminal << "\n";
	terminal << "DS: ";
	writeHex(registers.ds);
	terminal << "\n";
	terminal << "RDI: ";
	writeHex(registers.rdi);
	terminal << " | RSI: ";
	writeHex(registers.rsi);
	terminal << " | RBP: ";
	writeHex(registers.rbp);
	terminal << " | RSP: ";
	writeHex(registers.rsp);
	terminal << "\n";
	terminal << "RBX: ";
	writeHex(registers.rbx);
	terminal << " | RDX: ";
	writeHex(registers.rdx);
	terminal << " | RCX: ";
	writeHex(registers.rcx);
	terminal << " | RAX: ";
	writeHex(registers.rax);
	terminal << "\n";
	terminal << "RIP: ";
	writeHex(registers.rip);
	terminal << " | CS: ";
	writeHex(registers.cs);
	terminal << "\n";
	terminal << "RFLAGS: ";
	writeHex(registers.rflags);
	terminal << " | USERRSP: ";
	writeHex(registers.userrsp);
	terminal << "\n";
	terminal << "SS: ";
	writeHex(registers.ss);
	terminal << "\n";
	while (true);
}


void InterruptHandler(Registers& registers)
{
	if (registers.interruptNumber < 32)
	{
		ExceptionHandler(registers);
		return;
	}

	Terminal& terminal = Terminal::instance();

	if (registers.interruptNumber == 33)
	{
		uint8_t keycode = inb(0x60);

		if (keycode < 0 || keycode == prevKeyCode || keycode == 32)
			return;

		terminal.put_char(getChar(keycode), 15);

		prevKeyCode = keycode;
	}

//	terminal << "Interrupt: ";
//	terminal << registers.interruptNumber;
//	terminal << Terminal::EOL;
}

void ISRHandler(Registers& registers)
{
    InterruptHandler(registers);
}

void IRQHandler(Registers& registers)
{
    InterruptHandler(registers);
    if (registers.interruptNumber >= 40)
        outb(0xA0, 0x20); /* slave */
    outb(0x20, 0x20); /* master */
}

}
