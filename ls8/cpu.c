#include "cpu.h"
#include <stdio.h>

#define DATA_LEN 6


unsigned char cpu_ram_read(struct cpu *cpu, unsigned char mar)
{
  return cpu->ram[mar];
}

void cpu_ram_write(struct cpu *cpu, unsigned char mar, unsigned char mdr)
{
  cpu->ram[mar] = mdr;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filename)
{
  FILE *fp;
  char data[1024];
  int address = 0;
  
  fp = fopen(filename, "r");

  while (fgets(data, sizeof data, fp) != NULL) {
    unsigned char code = strtoul(data, NULL, 2);
    if(data[0] == '\n' || data[0] == '#') {
      continue;
    }
    cpu->ram[address++] = code;
  }
  fclose(fp);
  // TODO: Replace this with something less hard-coded
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
    break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  // Stack pointer 11110100 - 244 - F4
  unsigned char stack_p = 0xF4;
  cpu->registers[7] = stack_p;
  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    // 2. switch() over it to decide on a course of action.
    // 3. Do whatever the instruction should do according to the spec.
    // 4. Move the PC to the next instruction.
    unsigned char IR = cpu_ram_read(cpu, cpu->PC);
    
  
    unsigned char operand_a = cpu_ram_read(cpu, (cpu->PC + 1) & 0xff);
    unsigned char operand_b = cpu_ram_read(cpu, (cpu->PC + 2) & 0xff);
    int add_to_pc = (IR >> 6) + 1;

    switch (IR)
    {
    case LDI:
      printf("LDI: R%d: stored value: %d\n\n", operand_a, operand_b);
      cpu->registers[operand_a] = operand_b;
      break;
    case PRN:
      printf("PRN: R%d: value is %d\n\n", operand_a, cpu->registers[operand_a]);
      break;
    case HLT:
      printf("HLT: Program has halted\n\n");
      running = 0;
      exit(0);
    case MUL:
      alu(cpu, ALU_MUL, operand_a, operand_b);
      break;
    case PUSH:
      cpu->registers[7] -= 1;
      // printf("PUSH: ram: %d for R%d\n\n", cpu->registers[7], operand_a);
      cpu_ram_write(cpu, cpu->registers[7], cpu->registers[operand_a]);
      break;
    case POP:
      cpu->registers[operand_a] = cpu_ram_read(cpu, cpu->registers[7]);
      cpu->registers[7] += 1;
      // printf("POP: in ram: for R%d\n\n", cpu->registers[7], operand_a);
      break;
    default:
      printf("instruction does not exist\n\n");
      exit(1);
    }
      cpu->PC += add_to_pc;
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->PC = 0;
  // TODO: Zero registers and RAM
  memset(cpu->ram,0, sizeof cpu->ram);
  memset(cpu->registers, 0, sizeof cpu->registers);

  cpu->registers[7] = 0b11110100;
}
