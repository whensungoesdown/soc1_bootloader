CC=riscv32-unknown-linux-gnu-gcc
LD=riscv32-unknown-linux-gnu-ld
# -Qn no .comment section
CFLAGS=-I. -mstrict-align -march=rv32id -Qn -static 
DEPS =

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

loader: loader.o
	$(LD) --script=ldscript -N -o loader loader.o /home/uty/riscv32i/sysroot/usr/lib/libc.a

clean:
	rm -f ./*.o ./loader
