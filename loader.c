#include <stdio.h>
#include <string.h>
#include "soc1.h"

//-----------------------------------------------------------------------------//
void reset (void);
void trap (void);
void print_string (char* str, int len);
//-----------------------------------------------------------------------------//

char* hellostring =  "Hello World!";

//-----------------------------------------------------------------------------//
__attribute__((naked))
void takesomespace (void)
{

	asm("beq x0, x0, reset_stub");
	asm("beq x0, x0, trap_stub");
	asm __volatile__ (".long 0x00000000"); // .byte .word .long
	asm __volatile__ (".long 0x00000000");
	asm __volatile__ (".long 0x00000000");
	asm __volatile__ (".long 0x00000000");
	asm __volatile__ (".long 0x00000000");
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
		*(int*)(TEXT_VIDEO_RAM_START + 4 * i) = *(int*)(str + i * 4);
	}
}

inline
__attribute__((always_inline))
int read_csr(int csr_num)  
{
	int result;
	
	asm("csrr %0, %1" : "=r"(result) : "I"(csr_num));
	return result;
}
