# soc1_bootloader
bootloader C code for soc1 project

## Memory layout

`````````
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
`````````

* 0x0000 reset entry
* 0x0004 trap entry

The rest are some global variables.


## How to create the bootloader
After compiling the project, disassemble it using objdump. Put the following output into ram.mif.

Then change the line number and comments out all unnecessary content.

``````````
$ riscv32-unknown-linux-gnu-objdump -D -M no-aliases loader
[uty@u soc1_bootloader]$ riscv32-unknown-linux-gnu-objdump -D -M no-aliases loader

loader:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <takesomespace>:
   0:	02000863          	beq	zero,zero,30 <reset_stub>
   4:	02000e63          	beq	zero,zero,40 <trap_stub>
	...
  10:	0000                	c.unimp
  12:	0001                	c.addi	zero,0
	...
  2c:	00000013          	addi	zero,zero,0

00000030 <reset_stub>:
  30:	7ff00113          	addi	sp,zero,2047
  34:	00111113          	slli	sp,sp,0x1
  38:	024000ef          	jal	ra,5c <reset>
  3c:	0000006f          	jal	zero,3c <reset_stub+0xc>

00000040 <trap_stub>:
  40:	ff810113          	addi	sp,sp,-8
  44:	00112223          	sw	ra,4(sp)
  48:	064000ef          	jal	ra,ac <trap>
  4c:	00412083          	lw	ra,4(sp)
  50:	00810113          	addi	sp,sp,8
  54:	30200073          	mret
  58:	00000013          	addi	zero,zero,0

0000005c <reset>:
  5c:	ff010113          	addi	sp,sp,-16
  60:	00112623          	sw	ra,12(sp)
  64:	00812423          	sw	s0,8(sp)
  68:	01010413          	addi	s0,sp,16
  6c:	00000593          	addi	a1,zero,0
  70:	00200513          	addi	a0,zero,2
  74:	148000ef          	jal	ra,1bc <set_cursor_pos>
  78:	23002783          	lw	a5,560(zero) # 230 <hellostring>
  7c:	00c00593          	addi	a1,zero,12
  80:	00078513          	addi	a0,a5,0
  84:	094000ef          	jal	ra,118 <print_string>
  88:	000017b7          	lui	a5,0x1
  8c:	80078793          	addi	a5,a5,-2048 # 800 <trapstring+0x5cc>
  90:	30479073          	csrrw	zero,mie,a5
  94:	30045073          	csrrwi	zero,mstatus,8
  98:	00000013          	addi	zero,zero,0
  9c:	00c12083          	lw	ra,12(sp)
  a0:	00812403          	lw	s0,8(sp)
  a4:	01010113          	addi	sp,sp,16
  a8:	00008067          	jalr	zero,0(ra)

000000ac <trap>:
  ac:	fe010113          	addi	sp,sp,-32
  b0:	00112e23          	sw	ra,28(sp)
  b4:	00812c23          	sw	s0,24(sp)
  b8:	02010413          	addi	s0,sp,32
  bc:	fe042623          	sw	zero,-20(s0)
  c0:	000217b7          	lui	a5,0x21
  c4:	0007a783          	lw	a5,0(a5) # 21000 <trapstring+0x20dcc>
  c8:	fef42623          	sw	a5,-20(s0)
  cc:	00000593          	addi	a1,zero,0
  d0:	00300513          	addi	a0,zero,3
  d4:	0e8000ef          	jal	ra,1bc <set_cursor_pos>
  d8:	23402783          	lw	a5,564(zero) # 234 <trapstring>
  dc:	00600593          	addi	a1,zero,6
  e0:	00078513          	addi	a0,a5,0
  e4:	034000ef          	jal	ra,118 <print_string>
  e8:	00800593          	addi	a1,zero,8
  ec:	00300513          	addi	a0,zero,3
  f0:	0cc000ef          	jal	ra,1bc <set_cursor_pos>
  f4:	fec40793          	addi	a5,s0,-20
  f8:	00100593          	addi	a1,zero,1
  fc:	00078513          	addi	a0,a5,0
 100:	018000ef          	jal	ra,118 <print_string>
 104:	00000013          	addi	zero,zero,0
 108:	01c12083          	lw	ra,28(sp)
 10c:	01812403          	lw	s0,24(sp)
 110:	02010113          	addi	sp,sp,32
 114:	00008067          	jalr	zero,0(ra)

00000118 <print_string>:
 118:	fd010113          	addi	sp,sp,-48
 11c:	02812623          	sw	s0,44(sp)
 120:	03010413          	addi	s0,sp,48
 124:	fca42e23          	sw	a0,-36(s0)
 128:	fcb42c23          	sw	a1,-40(s0)
 12c:	fe042423          	sw	zero,-24(s0)
 130:	fe042623          	sw	zero,-20(s0)
 134:	fd842783          	lw	a5,-40(s0)
 138:	41f7d713          	srai	a4,a5,0x1f
 13c:	00377713          	andi	a4,a4,3
 140:	00f707b3          	add	a5,a4,a5
 144:	4027d793          	srai	a5,a5,0x2
 148:	00178793          	addi	a5,a5,1
 14c:	fef42423          	sw	a5,-24(s0)
 150:	fe042623          	sw	zero,-20(s0)
 154:	0480006f          	jal	zero,19c <print_string+0x84>
 158:	fec42783          	lw	a5,-20(s0)
 15c:	00279793          	slli	a5,a5,0x2
 160:	00078713          	addi	a4,a5,0
 164:	fdc42783          	lw	a5,-36(s0)
 168:	00e787b3          	add	a5,a5,a4
 16c:	01000713          	addi	a4,zero,16
 170:	00072703          	lw	a4,0(a4)
 174:	0007a783          	lw	a5,0(a5)
 178:	00f72023          	sw	a5,0(a4)
 17c:	01000793          	addi	a5,zero,16
 180:	0007a703          	lw	a4,0(a5)
 184:	01000793          	addi	a5,zero,16
 188:	00470713          	addi	a4,a4,4
 18c:	00e7a023          	sw	a4,0(a5)
 190:	fec42783          	lw	a5,-20(s0)
 194:	00178793          	addi	a5,a5,1
 198:	fef42623          	sw	a5,-20(s0)
 19c:	fec42703          	lw	a4,-20(s0)
 1a0:	fe842783          	lw	a5,-24(s0)
 1a4:	faf74ae3          	blt	a4,a5,158 <print_string+0x40>
 1a8:	00000013          	addi	zero,zero,0
 1ac:	00000013          	addi	zero,zero,0
 1b0:	02c12403          	lw	s0,44(sp)
 1b4:	03010113          	addi	sp,sp,48
 1b8:	00008067          	jalr	zero,0(ra)

000001bc <set_cursor_pos>:
 1bc:	fe010113          	addi	sp,sp,-32
 1c0:	00812e23          	sw	s0,28(sp)
 1c4:	02010413          	addi	s0,sp,32
 1c8:	fea42623          	sw	a0,-20(s0)
 1cc:	feb42423          	sw	a1,-24(s0)
 1d0:	fec42703          	lw	a4,-20(s0)
 1d4:	00070793          	addi	a5,a4,0
 1d8:	00279793          	slli	a5,a5,0x2
 1dc:	00e787b3          	add	a5,a5,a4
 1e0:	00479793          	slli	a5,a5,0x4
 1e4:	00078713          	addi	a4,a5,0
 1e8:	000107b7          	lui	a5,0x10
 1ec:	00f706b3          	add	a3,a4,a5
 1f0:	fe842783          	lw	a5,-24(s0)
 1f4:	00378793          	addi	a5,a5,3 # 10003 <trapstring+0xfdcf>
 1f8:	ffc7f713          	andi	a4,a5,-4
 1fc:	01000793          	addi	a5,zero,16
 200:	00e68733          	add	a4,a3,a4
 204:	00e7a023          	sw	a4,0(a5)
 208:	00000013          	addi	zero,zero,0
 20c:	01c12403          	lw	s0,28(sp)
 210:	02010113          	addi	sp,sp,32
 214:	00008067          	jalr	zero,0(ra)

Disassembly of section .rodata:

00000218 <.rodata>:
 218:	6548                	c.flw	fa0,12(a0)
 21a:	6c6c                	c.flw	fa1,92(s0)
 21c:	6f57206f          	jal	zero,73110 <trapstring+0x72edc>
 220:	6c72                	c.flwsp	fs8,28(sp)
 222:	2164                	c.fld	fs1,192(a0)
 224:	0000                	c.unimp
 226:	0000                	c.unimp
 228:	7254                	c.flw	fa3,36(a2)
 22a:	7061                	c.lui	zero,0xffff8
 22c:	203a                	c.fldsp	ft0,392(sp)
	...

Disassembly of section .sdata:

00000230 <hellostring>:
 230:	0218                	c.addi4spn	a4,sp,256
	...

00000234 <trapstring>:
 234:	0228                	c.addi4spn	a0,sp,264
	...
``````````
