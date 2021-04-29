#ifndef __VM_H
#define __VM_H

#include "codegen.h"

void virtual_machine(Instruction *code, int vFlag);
int getInstructionLength(Instruction* list);

#endif
