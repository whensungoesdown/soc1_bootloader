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
char* loadcodestring = "Code uploaded.";
char* executestring = "Execute user code...";
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
	asm __volatile__ (".long 0x00000400"); // 0x0018   USER_CODE_SIZE  1kb
	asm __volatile__ (".long 0x00000000"); // 0x001C   USER_CODE_LOADED
	asm __volatile__ (".long 0x00000000"); // 0x0020   BYTE_RECEIVED
	asm __volatile__ (".long 0x00000000"); // 0x0024   DWORD_CNT
	asm __volatile__ (".long 0x00000000"); // 0x0028   DWORD_TMP
	                                       // 0x002C
}


__attribute__((naked))
void reset_stub (void)
{
	asm("addi x2, x0, 0xFF"); // reset use user stack, later it calls into received code
	asm("slli x2, x2, 0x5");
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

	//
	// enable external interrupt
	//
	write_csr(0x304, 0x800);
	write_csr(0x300, 0x8);

	// call user code
	while (1)
	{
		if (1 == *(int*)USER_CODE_LOADED)
		{
			set_cursor_pos(7, 0);
			print_string(executestring, 20);
			((void (*) (void)) USER_START) (); // 0x1000
		}

	}
}

void trap (void)
{
	int n = 0;
	int nUserCodeSize = 0;
	int* pTmp = 0;
	int* pCnt = 0;
	int* pByteReceived = 0;

	n = *(int*)UARTDR;

	//
	// if user code uploaded, then stops receiving code
	// 
	//
	if (1 == *(int*)USER_CODE_LOADED)
	{
		//
		// if user set key handler, calls it
		//
		if (0 != *(int*)USER_KEY_HANDLER)
		{
			((void (*) (void)) (*(int*)USER_KEY_HANDLER)) ();
		}
		return;
	}

	set_cursor_pos(3, 0);
	print_string(trapstring, 6);
	set_cursor_pos(3, 8);
	print_string((char*)&n, 1);

	nUserCodeSize = *(int*)USER_CODE_SIZE;

	pTmp = (int*)DWORD_TMP;
	pCnt = (int*)DWORD_CNT;
	pByteReceived = (int*)BYTE_RECEIVED;


	if (0 == *pCnt)
	{
		*pTmp |= n;
	}
	else if (1 == *pCnt)
	{
		n <<= 8;
		*pTmp |= n;
	}
	else if (2 == *pCnt)
	{
		n <<= 16;
		*pTmp |= n;
	}
	else if (3 == *pCnt)
	{
		n <<= 24;
		*pTmp |= n;
	}
	else
	{
		// should not be here
	}
	
	*pCnt += 1;
	if (*pCnt >= 4)
	{
		*(int*)(USER_START + *pByteReceived) = *pTmp;

		*pCnt = 0;
		*pTmp = 0;
		*pByteReceived += 4;
	}


	if (*pByteReceived >= nUserCodeSize)
	{
		set_cursor_pos(5, 0);
		print_string(loadcodestring, 14);
		
		// code loaded
		*(int*)USER_CODE_LOADED = 1;
	}
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
