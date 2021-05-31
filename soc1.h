#ifndef _SOC1_H_
#define _SOC1_H_
//-----------------------------------------------------------------------------//
// not vector, 32-bit branch instruction
#define SYS_RESET		0x0000
#define SYS_TRAP		0x0004
//-----------------------------------------------------------------------------//

// Global data
#define CURSOR_POS		0x0010  // cursor position used by reset and trap
#define USER_KEY_HANDLER	0x0014  // set by the user code
#define USER_CODE_SIZE		0x0018
#define USER_CODE_LOADED	0x001C  // 0 no, 1 yes
// tmp for bootloader
#define BYTE_RECEIVED		0x0020
#define DWORD_CNT		0x0024
#define DWORD_TMP		0x0028


#define SYS_START		0x0000
#define SYS_END			0x0FFF
#define SYS_STACK		SYS_END

#define USER_START		0x01000
#define USER_END		0x0FFFF
#define USER_STACK		USER_END

//-----------------------------------------------------------------------------//
#define TEXT_VIDEO_RAM_START	0x10000
#define TEXT_VIDEO_RAM_END	0x107FF
#define UARTDR			0x21000
//-----------------------------------------------------------------------------//
#define TEXT_COLUMN		80
#define TEXT_ROW		30
//-----------------------------------------------------------------------------//
#endif
