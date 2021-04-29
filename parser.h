#ifndef __PARSER_H
#define __PARSER_H

#include "lex.h"

typedef struct symbol
{
	int kind;
	char* name;
	int val;
	int level;
	int addr;
	int mark;
	
} symbol;

symbol* parse(lexeme *list);
void Program();
void Block(int lexlevel);
void Error (int code);
int ConstDeclaration(int lexlevel);
int VarDeclaration(int lexlevel);
int ProcDeclaration (int lexlevel);
void Statement(int lexlevel);
void addToSymbol(int kind, char* name, int value, int l, int m, int mark);
int checkFor();
int checkForUnmarkedAndLex(int lexlevel);
int reverseSearch();
void markLastNumSymbols(int numSymbols);
int SymbolType(char* name);
void Condition(int lexlevel);
void Expression(int lexlevel);
void Term(int lexlevel);
void Factor(int lexlevel);

#endif
