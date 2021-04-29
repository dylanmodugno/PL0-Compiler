#ifndef __CODEGEN_H
#define __CODEGEN_H

#include "lex.h"
#include "parser.h"

typedef struct
{
	int op, r, l, m;
}Instruction;

Instruction* generate_code(symbol *table, lexeme *list);
void getTableLength();
void emit (int op, int r, int l, int m);
void CodeGenProgram();
void CodeGenBlock(int lexlevel);
void CodeGenStatement(int lexlevel);
int SymbolAddress(char* name);
int ConstantValue(char* name);
void CodeGenCondition(int lexlevel);
void CodeGenExpression(int regtoendupin, int lexlevel);
void CodeGenTerm(int regtoendupin, int lexlevel);
void CodeGenFactor(int regtoendupin, int lexlevel);
int SymbolTableIndex(char* name);
int GetProcM(int value);
void unMarkLastNumSymbols(int numSymbols);
void CodeGenMarkLastNumSymbols(int numSymbols);
int reverseProcSearch();
int reverseIndexSearch(int lexlevel);


#endif
