#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// general settings
#define STACK_SIZE 100
#define BYTES 50

// translation macros
#define LEFT "dec rsi\n"
#define RIGHT "inc rsi\n"
#define INC "mov bl, [rsi]\ninc bl\nmov [rsi], bl\n"
#define DEC "mov bl, [rsi]\ndec bl\nmov [rsi], bl\n"
#define BLOOP ".loop%d:\n"
#define SLOOP "mov bl, [rsi]\ncmp bl, 0\njnz .loop%d\n"
#define OUT "call printchar\n"
#define IN "call printchar\n"
// NEED TO CHANGE LATER

typedef struct IStack {
  int* stack;
  int stack_pointer;
  int size;
} IStack;

IStack* setupIStack(int size) {
  IStack* stack = malloc(sizeof(IStack));
  stack->stack = malloc(size * sizeof(int));
  stack->size = size;
  return stack;
}

int pushIStack(IStack* stack, int x) {
  if (stack->stack_pointer >= stack->size-1) {
    return -1;
  }
  *(stack->stack + stack->stack_pointer) = x;
  stack->stack_pointer++;
  return x;
}

int popIStack(IStack* stack) {
  if (stack->stack_pointer <= 0) {
    return -1;
  }
  stack->stack_pointer--;
  return *(stack->stack + stack->stack_pointer);
}

void destroyIStack(IStack* stack) {
  free(stack->stack);
  free(stack);
}


void setup(FILE *ptr, int bytes) {
  fprintf(ptr, "section .data\nmemory times %d db 0\n\nsection .text\nglobal _start\n\n", bytes);
  fputs("printchar:\npush rsi\npush rdi\npush rdx\n\nmov rax, 1\nmov rdi, 1\nmov rdx, 1\nsyscall\n\npop rdx\npop rdi\npop rsi\nret", ptr);
  fputs("\n\n_start:\n\nmov rsi, memory\n", ptr);
}

void end(FILE *ptr) {
  fputs("\nmov rax, 60\nmov rdi, 0\nsyscall\n", ptr);
}

void left(FILE *ptr) {
  fputs(LEFT, ptr);
}
void right(FILE *ptr) {
  fputs(RIGHT, ptr);
}
void inc(FILE *ptr) {
  fputs(INC, ptr);
}
void dec(FILE *ptr) {
  fputs(DEC, ptr);
}

void out(FILE *ptr) {
  fputs(OUT, ptr);
}

void in(FILE *ptr) {
  fputs(IN, ptr);
}

void bloop(FILE *ptr, int lnum) {
  fprintf(ptr, BLOOP, lnum);
}

void sloop(FILE *ptr, int lnum) {
  fprintf(ptr, SLOOP, lnum);
}

int main(int argc, char* argv[]) {

  // Creates a stack for the loop counter
  IStack* stack = setupIStack(STACK_SIZE);

  // Test if allocation failed
  if (stack == NULL) {
    fprintf(stderr, "malloc failed: %s\n", strerror(errno));
    return 1;
  }

  FILE * input = NULL;
  FILE * output = NULL;

  // test if file input is given
  if (argc > 1) {
    input = fopen(argv[1], "r"); 
  } else {
    printf("usage: COMMAND [FILE]\n");
    return 1;
  }

  output = fopen("out.asm", "w");

  // setup asm file
  setup(output, 40);

  int lcounter = 0;
  char cchar = 0;
  // parse every token
  while ( cchar != EOF ) {
    cchar = fgetc(input);

    switch (cchar) {
      case '<':
        left(output);
        break;
      case '>':
        right(output);
        break;
      case '+':
        inc(output);
        break;
      case '-':
        dec(output);
        break;
      case '.':
        out(output);
        break;
      case ',':
        in(output);
        break;
      case '[':
        if(pushIStack(stack, lcounter) < 0) {
          break;
        };
        bloop(output, lcounter);
        lcounter++;
        break;
      case ']':
        int lname = popIStack(stack);
        if (lname < 0) {
          break;
        }
        sloop(output, lname);
        break;
      case '\n':
      case '\r':
      case '\0':
      case EOF:
        break;
      default:
        fprintf(stderr, "invalid character: %c (%d)\n", cchar, cchar);
        break;
    }
  }
  // end asm file
  end(output);

  // clean up memory
  fclose(input);
  fclose(output);
  destroyIStack(stack);
  return EXIT_SUCCESS;
}

