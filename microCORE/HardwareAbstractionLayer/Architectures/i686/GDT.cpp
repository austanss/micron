#include "AssemblyFunctions.h"
#include "GDT.h"
#include "Terminal.h"
#include "IO.h"
#include "KernelUtil.h"

uint8_t prevKeyCode = 156;

extern "C"
{

extern void flushGDT(uint32_t);
extern void flushIDT(uint32_t);
extern void initGDT();
extern void gdtSetGate(int32_t, uint32_t, uint32_t, uint8_t, uint8_t);

GDTEntry gdtEntries[5];
GDTPointer gdtPointer;

IDTEntry   idtEntries[256];
IDTPointer idtPointer;

void loadGDT()
{
	auto gdt_set_gate = [](int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
	{
		gdtEntries[num].base_low    = (base & 0xFFFF);
		gdtEntries[num].base_middle = (base >> 16) & 0xFF;
		gdtEntries[num].base_high   = (base >> 24) & 0xFF;

		gdtEntries[num].limit_low   = (limit & 0xFFFF);
		gdtEntries[num].granularity = (limit >> 16) & 0x0F;

		gdtEntries[num].granularity |= gran & 0xF0;
		gdtEntries[num].access      = access;
	};

	gdtPointer.limit = (sizeof(GDTEntry) * 5) - 1;
	gdtPointer.base = (uint32_t) &gdtEntries;

	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	flushGDT((uint32_t)&gdtPointer);
}

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

void loadIDT()
{
	auto idt_set_gate = [](uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
	{
		idtEntries[num].base_lower = base & 0xFFFF;
		idtEntries[num].base_upper = (base >> 16) & 0xFFFF;

		idtEntries[num].sel     = sel;
		idtEntries[num].always0 = 0;
		idtEntries[num].flags   = flags;
	};

	idtPointer.limit = sizeof(IDTEntry) * 256 -1;
	idtPointer.base  = (uint32_t)&idtEntries;

	memset(&idtEntries, 0, sizeof(IDTEntry)*256);

	idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
	idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);
	idt_set_gate(2,  (uint32_t)isr2,  0x08, 0x8E);
	idt_set_gate(3,  (uint32_t)isr3,  0x08, 0x8E);
	idt_set_gate(4,  (uint32_t)isr4,  0x08, 0x8E);
	idt_set_gate(5,  (uint32_t)isr5,  0x08, 0x8E);
	idt_set_gate(6,  (uint32_t)isr6,  0x08, 0x8E);
	idt_set_gate(7,  (uint32_t)isr7,  0x08, 0x8E);
	idt_set_gate(8,  (uint32_t)isr8,  0x08, 0x8E);
	idt_set_gate(9,  (uint32_t)isr9,  0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
	idt_set_gate(32, (uint32_t)irq0,  0x08, 0x8E);     
        idt_set_gate(33, (uint32_t)irq1,  0x08, 0x8E);     
        idt_set_gate(34, (uint32_t)irq2,  0x08, 0x8E);     
        idt_set_gate(35, (uint32_t)irq3,  0x08, 0x8E);     
        idt_set_gate(36, (uint32_t)irq4,  0x08, 0x8E);     
        idt_set_gate(37, (uint32_t)irq5,  0x08, 0x8E);     
        idt_set_gate(38, (uint32_t)irq6,  0x08, 0x8E);     
        idt_set_gate(39, (uint32_t)irq7,  0x08, 0x8E);     
        idt_set_gate(40, (uint32_t)irq8,  0x08, 0x8E);     
        idt_set_gate(41, (uint32_t)irq9,  0x08, 0x8E);     
        idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);     
        idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);     
        idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);     
        idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);     
        idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);     
        idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);     
        
	flushIDT((uint32_t)&idtPointer);
}

struct Registers
{
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t interruptNumber, errorCode;
	uint32_t eip, cs, eflags, useresp, ss;
};


void ExceptionHandler(Registers& registers)
{
	Terminal& terminal = Terminal::instance();

	terminal << "$RED!ERROR: $WHITE!" << registers.interruptNumber << " -/- " << registers.errorCode << Terminal::EOL;
	terminal << "DS: " << registers.ds << Terminal::EOL;
	terminal << "EDI: " << registers.edi << " | ESI: " << registers.esi << " | EBP: " << registers.ebp << " | ESP: " << registers.esp << Terminal::EOL;
	terminal << "EBX: " << registers.ebx << " | EDX: " << registers.edx << " | ECX: " << registers.ecx << " | EAX:" << registers.eax << Terminal::EOL;
	terminal << "EIP: " << registers.eip << " | CS: " << registers.cs << Terminal::EOL;
	terminal << "EFLAGS: " << registers.eflags << " | USERESP: " << registers.useresp << Terminal::EOL;
	terminal << "SS: " << registers.ss << Terminal::EOL;
}


void InterruptHandler(Registers& registers)
{
	if (registers.interruptNumber < 32)
	{
		ExceptionHandler(registers);
		return;
	}

	Terminal& terminal = Terminal::instance();

	if (registers.interruptNumber == 1 || registers.interruptNumber == 33)
	{
		uint8_t keycode = inb(0x60);

		if (keycode < 0 || keycode == prevKeyCode || keycode == 32)
			return;

		writeHex(keycode);

		terminal << ", " << keycode << ", " << getChar(keycode) << Terminal::EOL;

		prevKeyCode = keycode;
	}

	terminal << "Interrupt: ";
	terminal << registers.interruptNumber;
	terminal << Terminal::EOL;
}

void ISRHandler(Registers& registers)
{
	outb(0xA0, 0x20);
	InterruptHandler(registers);
	outb(0x20, 0x20);
}

void IRQHandler(Registers& registers)
{
    	if (registers.interruptNumber >= 40) outb(0xA0, 0x20); /* slave */
	InterruptHandler(registers);
   	outb(0x20, 0x20); /* master */
}

void loadTables()
{
	loadGDT();
	loadIDT();
}

}
