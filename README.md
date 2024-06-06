

### Just a Simple Brainfuck Compiler (only Linux, only 64 bit) ###

Its not the best compiler out there, but it's ok for playing around with the Brainfuck Language.

Compile the compiler (gcc needed):
```
make
```

Compile a Brainfuck file (hello.bf sample file):
```
./bfc hello.bf
```

After compiling you get an 64bit assembly file named "out.asm".
You can assemble it with an assembler like nasm:
```
nasm -f elf64 out.asm
```

Then all you have to do is link it:
```
ld -o [YOUR_PROGRAMM_NAME] out.asm
```

And then you have a 64bit ELF binary.

