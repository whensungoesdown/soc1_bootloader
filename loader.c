#include <stdio.h>
#include <string.h>
#include "soc1.h"

//-----------------------------------------------------------------------------//
#define read_csr(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define write_csr(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define swap_csr(reg, val) ({ unsigned long __tmp; \
  asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "rK"(val)); \
  __tmp; })

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })
//-----------------------------------------------------------------------------//

void reset (void);
void trap (void);
void print_string (char* str, int len);
void set_cursor_pos (int row, int column);
//-----------------------------------------------------------------------------//

char* hellostring =  "Hello World!";
char* trapstring = "Trap: ";
//-----------------------------------------------------------------------------//
__attribute__((naked))
void takesomespace (void)
{
	// .byte .word .long
	asm("beq x0, x0, reset_stub");         // 0x0000
	asm("beq x0, x0, trap_stub");          // 0x0004		
	asm __volatile__ (".long 0x00000000"); // 0x0008
	asm __volatile__ (".long 0x00000000"); // 0x000c
	asm __volatile__ (".long 0x00010000"); // 0x0010   CURSOR_POS
	asm __volatile__ (".long 0x00000000"); // 0x0014   USER_KEY_HANDLER
	asm __volatile__ (".long 0x00000000"); // 0x0018   USER_CODE_LOAD
	asm __volatile__ (".long 0x00000000"); // 0x001C   USER_CODE_COUNT
	asm __volatile__ (".long 0x00000000"); // 0x0020
	asm __volatile__ (".long 0x00000000"); // 0x0024
	asm __volatile__ (".long 0x00000000"); // 0x0028
	                                       // 0x002C
}


__attribute__((naked))
void reset_stub (void)
{
	asm("addi x2, x0, 0x7FF");
	asm("slli x2, x2, 0x1");
	reset();
	while (1) {};
}

__attribute__((naked))
void trap_stub (void)
{
	//asm("addi x2, x0, 0x7FF");
	//asm("slli x2, x2, 0x1");
	asm("addi sp,sp,-8");
	asm("sw	  ra,4(sp)");
	trap();
	asm("lw	  ra,4(sp)");
	asm("addi sp,sp,8");
	asm("mret");
}

void reset (void)
{
	set_cursor_pos(2, 0);
	print_string(hellostring, 12);
	write_csr(0x304, 0x800);
	write_csr(0x300, 0x8);
}

void trap (void)
{
	int n = 0;

	n = *(int*)UARTDR;

	set_cursor_pos(3, 0);
	print_string(trapstring, 6);
	set_cursor_pos(3, 8);
	print_string((char*)&n, 1); 
}



void print_string (char* str, int len)
{
	int n = 0;
	int i = 0;

	n = len / 4 + 1;

	for (i = 0; i < n; i++)
	{
		*(int*)(*(int*)CURSOR_POS) = *(int*)(str + i * 4);
		*(int*)CURSOR_POS = *(int*)CURSOR_POS + 4;
	}
}

void set_cursor_pos (int row, int column)
{
	*(int*)CURSOR_POS = TEXT_VIDEO_RAM_START + row * TEXT_COLUMN + ((column + 3) & ~0x3); // 4-byte align
}

//inline
//__attribute__((always_inline))
//int read_csr(int csr_num)  
//{
//	int result;
//	
//	asm("csrr %0, %1" : "=r"(result) : "I"(csr_num));
//	return result;
//}


//inline
//__attribute__((always_inline))
//void write_csr(int csr_num, int val)  
//{
//	//asm("csrw %0, %1" : "L"(csr_num) : "rK"(val));
//	asm volatile ("csrw " ##csr_num ", %0" :: "rK"(val));
//}
