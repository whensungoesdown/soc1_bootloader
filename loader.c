#include <stdio.h>
#include <string.h>
#include "soc1.h"

//-----------------------------------------------------------------------------//
void reset (void);
void trap (void);
void print_string (char* str, int len);
void set_cursor_pos (int row, int column);
//-----------------------------------------------------------------------------//

char* hellostring =  "Hello World!";

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

void reset (void)
{
	set_cursor_pos(2, 0);
	print_string(hellostring, 12);
}


__attribute__((naked))
void trap_stub (void)
{
	asm("addi x2, x0, 0x7FF");
	asm("slli x2, x2, 0x1");
	trap();
	asm("mret");
}

void trap (void)
{

}



void print_string (char* str, int len)
{
	int n = 0;
	int i = 0;

	n = len / 4;

	for (i = 0; i < n; i++)
	{
		*(int*)(*(int*)CURSOR_POS) = *(int*)(str + i * 4);
		*(int*)CURSOR_POS = *(int*)CURSOR_POS + 4;
	}
}

void set_cursor_pos (int row, int column)
{
	*(int*)CURSOR_POS = TEXT_VIDEO_RAM_START + row * TEXT_COLUMN + column;
}

inline
__attribute__((always_inline))
int read_csr(int csr_num)  
{
	int result;
	
	asm("csrr %0, %1" : "=r"(result) : "I"(csr_num));
	return result;
}
