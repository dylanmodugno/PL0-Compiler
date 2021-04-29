#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"


lexeme* TokenList;
int TokenIndex;
symbol *symTable;
int tableLen = 0;
Instruction *code;
int cx = 0;
int savedCodeIndexJump;
int savedCodeIndexCondition;

Instruction* generate_code(symbol *table, lexeme *list)
{
	code = malloc(500 * sizeof(Instruction));
	TokenList = list;
	symTable = table;
	getTableLength();
	TokenIndex = 0;
	CodeGenProgram();
	return code;
}


/*
PROGRAM
	i = 1
	foreach symbol in symboltable
		if symbol is procedure
			symbol.value = i
			i++					
			emit a JMP (7, 0, 0, 0)
	BLOCK (0)
	for i = 0; code[i].opcode == 7 (jumps); i++
		code[i].m = the m value from that procedure's symbol table entry
	foreach line in code
		if line.opcode == 5 (calls)
			find procedure in the symbol table with value line.m
			line.m = the m value from that symbol table entry
	emit HALT (9, 0, 0, 3)
*/
void CodeGenProgram()
{
	/*
	emit(7, 0, 0, 1);
	CodeGenBlock();
	emit(9, 0, 0, 3);
	*/
	int i = 1, j = 0, k = 1;
	getTableLength();
	for(j = 0; j < tableLen; j++)
	{
		if (symTable[j].kind == 3)
		{
			symTable[j].val = i;
			i++;
			emit(7, 0, 0, 0);
		}
	}
	CodeGenBlock(0);
	for(j = 0; code[j].op == 7; j++)
	{
		if (k <= i)
		{
			code[j].m = GetProcM(k);
			k++;
		}
	}
	for (j = 0; j < cx; j++)
	{
		if (code[j].op == 5)
		{
			code[j].m = GetProcM(code[j].m);
		}
	}
	emit(9, 0, 0, 3);
}

/*
BLOCK (lexlevel)
	numSymbols = 0
	numVars = 0
	if token == const
		do
			token + 1
			numSymbols++
			unmark the const in the symbol table
			token + 3
		while token == ,
		token + 1
	if token == var
		do 
			token + 1
			numVars++
			numSymbols++
			unmark the var in the symbol table
			token + 1
		while token == ,
		token + 1
	if token == procedure
		do
			token + 1
			numSymbols++
			unmark the procedure in the symbol table
			token + 2
			BLOCK (lexlevel + 1)
			emit RTN (2, 0, 0, 0)
			token + 1
		while token == procedure
	update the symbol table entry for this procedure (current procedure) M = current code index
	emit INC (6, 0, 0, 3 + numVars)
	STATEMENT (lexlevel)
	mark the last numSymbols number of unmarked symbols
*/
void CodeGenBlock(int lexlevel)
{
	int numSymbols = 0;
	int numVars = 0;
	int procIndex = 0;
	if (TokenList[TokenIndex].type == constsym)
	{
		do
		{
			TokenIndex++;
			numSymbols++;
			symTable[SymbolTableIndex(TokenList[TokenIndex].ident)].mark = 0;
			TokenIndex += 3;
			
		} while(TokenList[TokenIndex].type == commasym);
		TokenIndex++;
	}
	if (TokenList[TokenIndex].type == varsym)
	{
		do
		{
			TokenIndex++;
			numVars++;
			numSymbols++;
			symTable[SymbolTableIndex(TokenList[TokenIndex].ident)].mark = 0;
			TokenIndex++;
			
		} while(TokenList[TokenIndex].type == commasym);
		TokenIndex++;
	}
	if (TokenList[TokenIndex].type == procsym)
	{
		do
		{
			TokenIndex++;
			numSymbols++;
			procIndex = SymbolTableIndex(TokenList[TokenIndex].ident);
			symTable[procIndex].mark = 0;
			TokenIndex += 2;
			CodeGenBlock(lexlevel + 1);
			emit(2, 0, 0, 0);
			TokenIndex++;
		
		} while(TokenList[TokenIndex].type == procsym);
		TokenIndex++;
	}
	symTable[procIndex].addr = cx;
	emit(6, 0, 0, numVars + 3);
	CodeGenStatement(lexlevel);
	CodeGenMarkLastNumSymbols(numSymbols);
}
/*
STATEMENT (lexlevel)
	if token == ident
		save the symbol table index of the var entry unmarked and closest in lexlevel
		token + 2
		EXPRESSION (0, lexlevel)
		emit STO (4, 0, L comes from the symbol table, M comes from the symbol table)
	if token == call
		token + 1
		save the symbol table index of the procedure entry unmarked and closest in lexlevel
		emit CAL (5, 0, L = lexlevel - L from the symbol table, M = value from the symbol table entry)
		token + 1
	if token == begin
		token + 1
		STATEMENT (lexlevel)
		whlie token == ;
			token + 1
			STATEMENT (lexlevel)
		token + 1
	if token == if
		token + 1
		CONDITION (lexlevel)
		save the code index for jpc
		emit JPC (8, 0, 0, 0)
		token + 1
		STATEMENT (lexlevel)
		if token == else
			token + 1
			save the current code index for jmp
			emit JMP (7, 0, 0, 0)
			fix the jpc from earlier, savedCodeIndexForJPC.M = currentcodeindex
			STATEMENT (lexlevel)
			fix the jmp from earlier, savedCodeIndexForJMP.M = currentcodeindex
		else (ie there was no "else" keyword)
			fix the jpc from earlier, savedCodeIndexForJPC.M = currentcodeindex
	if token == while
		token + 1
		save the code index for condition
		CONDITION (lexlevel)
		token + 1
		save the code index for jump
		emit JPC (8, 0, 0, 0)
		STATEMENT (lexlevel)
		emit JMP (7, 0, 0, savedCodeIndexForCondition)
		fix the JPC from earlier, savedCodeIndexForJump.M = currentcodeindex
	if token == read
		token + 1
		save the symbol table index of the var entry unmarked and closest to current lexlevel
		token + 1
		emit READ (9, 0, 0, 2)
		emit STO (4, 0, L = lexlevel - L from the symbol table, M comes from the symbol table)
	if token == write
		token + 1
		EXPRESSION (lexlevel)
		emit WRITE (9, 0, 0, 1)
		token + 1
*/
void CodeGenStatement(int lexlevel)
{
	int savedIndex;
	/*
	if token == ident
		save the symbol table index of the var entry unmarked and closest in lexlevel
		token + 2
		EXPRESSION (0, lexlevel)
		emit STO (4, 0, L comes from the symbol table, M comes from the symbol table)
	*/
	if (TokenList[TokenIndex].type == identsym)
	{
		savedIndex = reverseProcSearch();
		//int M = SymbolAddress(TokenList[TokenIndex].ident);
		TokenIndex += 2;
		CodeGenExpression(0, lexlevel);
		emit(4, 0, symTable[savedIndex].level, symTable[savedIndex].addr);
	}
	/*
	if token == call
		token + 1
		save the symbol table index of the procedure entry unmarked and closest in lexlevel
		emit CAL (5, 0, L = lexlevel - L from the symbol table, M = value from the symbol table entry)
		token + 1
	*/
	if (TokenList[TokenIndex].type == callsym)
	{
		TokenIndex++;
		savedIndex = reverseProcSearch();
		emit(5, 0, symTable[savedIndex].level, symTable[savedIndex].addr); //Fix M value
		TokenIndex++;
	}

	/*
	if token == begin
		token + 1
		STATEMENT (lexlevel)
		whlie token == ;
			token + 1
			STATEMENT (lexlevel)
		token + 1
	*/
	if (TokenList[TokenIndex].type == beginsym)
	{
		TokenIndex++;
		CodeGenStatement(lexlevel);
		while(TokenList[TokenIndex].type == semicolonsym)
		{
			TokenIndex++;
			CodeGenStatement(lexlevel);
		}
		TokenIndex++;
	}
	/*
	if token == if
		token + 1
		CONDITION (lexlevel)
		save the code index for jpc
		emit JPC (8, 0, 0, 0)
		token + 1
		STATEMENT (lexlevel)
		if token == else
			token + 1
			save the current code index for jmp
			emit JMP (7, 0, 0, 0)
			fix the jpc from earlier, savedCodeIndexForJPC.M = currentcodeindex
			STATEMENT (lexlevel)
			fix the jmp from earlier, savedCodeIndexForJMP.M = currentcodeindex
		else (ie there was no "else" keyword)
			fix the jpc from earlier, savedCodeIndexForJPC.M = currentcodeindex
	*/
	if (TokenList[TokenIndex].type == ifsym)
	{
		TokenIndex++;
		CodeGenCondition(lexlevel);
		savedCodeIndexJump = cx;
		emit(8, 0, 0, 0);
		TokenIndex++;
		CodeGenStatement(lexlevel);
		if(TokenList[TokenIndex].type == elsesym)
		{
			TokenIndex++;
			int CurrentCodeIndex = cx;
			emit(7, 0, 0, 0);
			code[CurrentCodeIndex].m = cx;
			CodeGenStatement(lexlevel);
			code[savedCodeIndexJump].m = cx;
		}
		else
			code[savedCodeIndexJump].m = cx;
	}
	/*
	if token == while
		token + 1
		save the code index for condition
		CONDITION (lexlevel)
		token + 1
		save the code index for jump
		emit JPC (8, 0, 0, 0)
		STATEMENT (lexlevel)
		emit JMP (7, 0, 0, savedCodeIndexForCondition)
		fix the JPC from earlier, savedCodeIndexForJump.M = currentcodeindex
	*/
	if (TokenList[TokenIndex].type == whilesym)
	{
		TokenIndex++;
		savedCodeIndexCondition = cx;
		CodeGenCondition(lexlevel);
		TokenIndex++;
		savedCodeIndexJump = cx;
		emit(8, 0, 0, 0);
		CodeGenStatement(lexlevel);
		emit(7, 0, 0, savedCodeIndexCondition);
		code[savedCodeIndexJump].m = cx;
	}
	/*
	if token == read
		token + 1
		save the symbol table index of the var entry unmarked and closest to current lexlevel
		token + 1
		emit READ (9, 0, 0, 2)
		emit STO (4, 0, L = lexlevel - L from the symbol table, M comes from the symbol table)
	*/
	if (TokenList[TokenIndex].type == readsym)
	{
		TokenIndex++;
		int M = SymbolAddress(TokenList[TokenIndex].ident);
		TokenIndex++;
		emit(9, 0, 0, 2);
		emit(4, 0, 0, M);
	}
	/*
	if token == write
		token + 1
		EXPRESSION (lexlevel)
		emit WRITE (9, 0, 0, 1)
		token + 1
	*/
	if (TokenList[TokenIndex].type == writesym)
	{
		TokenIndex++;
		CodeGenExpression(0, lexlevel);
		emit(9, 0, 0, 1);
		TokenIndex++;
		
		/*
		int symbolIndex = SymbolTableIndex(TokenList[TokenIndex].ident);
		
		if (symTable[symbolIndex].kind == 1)
		{
			emit(1, regtoendupin, 0, ConstantValue(TokenList[TokenIndex].ident));
		}	
		if (symTable[symbolIndex].kind == 2)
		{
			int M = SymbolAddress(TokenList[TokenIndex].ident);
			emit(3, regtoendupin, 0, M);
		}
		*/
	}
}
/*		
CONDITION
	if token == odd
		token + 1
		EXPRESSION (0, lexlevel)
		emit ODD (15, 0, 0, 0)
	else
		EXPRESSION (0, lexlevel)
		if token == =
			token + 1
			EXPRESSION (1, lexlevel)
			emit EQL (17, 0, 0, 1)
		if token == <>
			token + 1
			EXPRESSION (1, lexlevel)
			emit NEQ (18, 0, 0, 1) 
		if token == <
			token + 1
			EXPRESSION (1, lexlevel)
			emit LSS (19, 0, 0, 1)
		if token == <=
			token + 1
			EXPRESSION (1, lexlevel)
			emit LEQ (20, 0, 0, 1)
		if token == >
			token + 1
			EXPRESSION (1, lexlevel)
			emit GTR (21, 0, 0, 1)
		if token == >=
			token + 1
			EXPRESSION (1, lexlevel)
			emit GEQ (22, 0, 0, 1)
*/

void CodeGenCondition(int lexlevel)
{
	if (TokenList[TokenIndex].type == oddsym)
	{
		TokenIndex++;
		CodeGenExpression(0, lexlevel);
		emit(15, 0, 0, 0);
	}
	else
	{
		CodeGenExpression(0, lexlevel);
		if (TokenList[TokenIndex].type == eqsym)
		{
			TokenIndex++;
			CodeGenExpression(1, lexlevel);
			emit(17, 0, 0, 1);
		}
		if (TokenList[TokenIndex].type == neqsym)
		{
			TokenIndex++;
			CodeGenExpression(1, lexlevel);
			emit(18, 0, 0, 1) ;	
		}
		
		if (TokenList[TokenIndex].type == lessym)
		{
			TokenIndex++;
			CodeGenExpression(1, lexlevel);
			emit(19, 0, 0, 1);
		}
		
		if (TokenList[TokenIndex].type == leqsym)
		{
			TokenIndex++;
			CodeGenExpression(1, lexlevel);
			emit(20, 0, 0, 1);
		}
		if (TokenList[TokenIndex].type == gtrsym)
		{
			TokenIndex++;
			CodeGenExpression(1, lexlevel);
			emit(21, 0, 0, 1);
		}
		
		if (TokenList[TokenIndex].type == geqsym)
		{
			TokenIndex++;
			CodeGenExpression(1, lexlevel);
			emit(22, 0, 0, 1);
		}		
	}
}

/*
EXPRESSION (regtoendupin = int where answer should go)
	if token == +
		token + 1
	if token == -
		token + 1
		TERM (regtoendupin, lexlevel)
		emit NEG (10, regtoendupin, 0, 0)
		while token == + || -
			if token == +
				token + 1
				TERM (regtoendupin + 1, lexlevel)
				emit ADD (11, regtoendupin, regtoendupin, regtoendupin + 1)
			if token == -
				token + 1
				TERM (regtoendupin + 1, lexlevel)
				emit SUB (12, regtoendupin, regtoendupin, regtoendupin + 1)
		return
	TERM (regtoendupin, lexlevel)
	while token == + || -
		if token == +
			token + 1
			TERM (regtoendupin + 1, lexlevel)
			emit ADD (11, regtoendupin, regtoendupin, regtoendupin + 1)
		if token == -
			token + 1
			TERM (regtoendupin + 1, lexlevel)
			emit SUB (12, regtoendupin, regtoendupin, regtoendupin + 1)
*/

void CodeGenExpression(int regtoendupin, int lexlevel)
{
	if (TokenList[TokenIndex].type == plussym)
		TokenIndex++;
	if (TokenList[TokenIndex].type == minussym)
	{
		TokenIndex++;
		CodeGenTerm(regtoendupin, lexlevel);
		emit(10, regtoendupin, 0, 0);
		while ((TokenList[TokenIndex].type == plussym) || TokenList[TokenIndex].type == minussym)
			if (TokenList[TokenIndex].type == plussym)
			{
				TokenIndex++;
				CodeGenTerm(regtoendupin + 1, lexlevel);
				emit(11, regtoendupin, regtoendupin, regtoendupin + 1);
			}
			if (TokenList[TokenIndex].type == minussym)
			{
				TokenIndex++;
				CodeGenTerm(regtoendupin + 1, lexlevel);
				emit(12, regtoendupin, regtoendupin, regtoendupin + 1);
			}
		return;
	}
	CodeGenTerm(regtoendupin, lexlevel);
	while ((TokenList[TokenIndex].type == plussym) || TokenList[TokenIndex].type == minussym)
	{
		if (TokenList[TokenIndex].type == plussym)
		{
			TokenIndex++;
			CodeGenTerm(regtoendupin + 1, lexlevel);
			emit(11, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		if (TokenList[TokenIndex].type == minussym)
		{
			TokenIndex++;
			CodeGenTerm(regtoendupin + 1, lexlevel);
			emit(12, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}	
}

/*		
TERM (regtoendupin)
	FACTOR (regtoendupin, lexlevel)
	while token == * || /
		if token == *
			token + 1
			FACTOR (regtoendupin + 1, lexlevel)
			emit MUL (13, regtoendupin, regtoendupin, regtoendupin + 1)
		if token == /
			token + 1
			FACTOR (regtoendupin + 1, lexlevel)
			emit DIV (14, regtoendupin, regtoendupin, regtoendupin + 1)
*/

void CodeGenTerm(int regtoendupin, int lexlevel)
{
	CodeGenFactor(regtoendupin, lexlevel);
	while ((TokenList[TokenIndex].type == multsym) || TokenList[TokenIndex].type == slashsym)
	{
		if (TokenList[TokenIndex].type == multsym)
		{
			TokenIndex++;
			CodeGenFactor(regtoendupin + 1, lexlevel);
			emit(13, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		if (TokenList[TokenIndex].type == slashsym)
		{
			TokenIndex++;
			CodeGenFactor(regtoendupin + 1, lexlevel);
			emit(14, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}
}

/*
FACTOR (regtoendupin)
	if token == ident
		save the symbol table index of the var or const entry unmarked and closest in lexlevel
		if token == const
			emit LIT (1, regtoendupin, 0, value from the symbol table)
		if token == var
			emit LOD (3, regtoendupin, L = lexlevel - L from the symbol table, m from the symbol table)
		token + 1
	else if token == number
		emit LIT (1, regtoendupin, 0, number value)
		token + 1
	else
		token + 1
		EXPRESSION (regtoendupin, lexlevel)
		token + 1
*/

void CodeGenFactor(int regtoendupin, int lexlevel)
{
	if (TokenList[TokenIndex].type == identsym)
	{
		int symbolIndex = SymbolTableIndex(TokenList[TokenIndex].ident);
		int symbolIndex2 = reverseProcSearch();
		
		if (symTable[symbolIndex].kind == 1)
		{
			emit(1, regtoendupin, 0, ConstantValue(TokenList[TokenIndex].ident));
		}	
		if (symTable[symbolIndex].kind == 2)
		{
			int M = SymbolAddress(TokenList[TokenIndex].ident);
			emit(3, regtoendupin, symTable[symbolIndex2].level, M);
		}
		TokenIndex++;	
	}
	else if (TokenList[TokenIndex].type == numbersym)
	{
		emit (1, regtoendupin, 0, TokenList[TokenIndex].val);
		TokenIndex++;
	}
	else
	{
		TokenIndex++;
		CodeGenExpression(regtoendupin, lexlevel);
		TokenIndex++;
	}
}

//Returns length of symbol table
void getTableLength()
{
	int i;
	while(symTable[i].kind != 0)
		i++;
	tableLen = i;
}

//Returns the address of a variable in the symbol table given the symbols name
int SymbolAddress(char* name)
{
	int i = 0;
	while(i < tableLen)
	{
		if (strcmp (symTable[i].name, name) == 0)
			return symTable[i].addr;
		i++;
	}
	return 0;
}

//Returns the value of a constant in the symbol table given the symbols name
int ConstantValue(char* name)
{
	int i = 0;
	while(i < tableLen)
	{
		if (strcmp (symTable[i].name, name) == 0)
			return symTable[i].val;
		i++;
	}
	return 0;
}

//Returns the index of a symbol in the symbol table given the symbols name
int SymbolTableIndex(char* name)
{
	int i = 0;
	while(i < tableLen)
	{
		if (strcmp (symTable[i].name, name) == 0)
			return i;
		i++;
	}
	return 999;
}

//Get a procedures M value
int GetProcM(int value)
{
	int i = 0;
	while(i < tableLen)
	{
		if (symTable[i].kind == 3 && symTable[i].val == value)
			return (symTable[i].addr);
		i++;
	}
	return 0;
}

//Unmarks the last numSymbols symbols
void unMarkLastNumSymbols(int numSymbols)
{
	int i = tableLen - 1;
	int lastIndex = tableLen - numSymbols;
	if(tableLen == 0)
		return;
	else
	{
		while(i >= lastIndex)
		{
			symTable[i].mark = 0;
			i--;
		}
	return;
	}
}

//Marks the last numSymbols symbols in symbol table
void CodeGenMarkLastNumSymbols(int numSymbols)
{
	int i = tableLen - 1;
	int lastIndex = tableLen - numSymbols;
	if(tableLen == 0)
		return;
	else
	{
		while(i >= lastIndex)
		{
			symTable[i].mark = 1;
			i--;
		}
	return;
	}
}

//Finds the index of the nearest procedure with the tokens name
int reverseProcSearch()
{
	//printf("\nReverse was ran looking for %s\n", tokenList[tokenIndex].ident);
	int i = tableLen - 1;
	if(tableLen == 0)
		return 0;
	else
	{
		while(i >= 0)
		{
			if (strcmp (symTable[i].name, TokenList[TokenIndex].ident) == 0 && symTable[i].mark == 0)
				return i;
			i--;
		}
	return -1;
	}
}

//Finds the index of the nearest var with the tokens name
int reverseIndexSearch(int lexlevel)
{
	//printf("\nReverse was ran looking for %s\n", tokenList[tokenIndex].ident);
	int i = tableLen - 1;
	if(tableLen == 0)
		return 0;
	else
	{
		while(i >= 0)
		{
			if (strcmp (symTable[i].name, TokenList[TokenIndex].ident) == 0 && symTable[i].mark == 0 && symTable[i].level <= lexlevel)
				return i;
			i--;
		}
	return -1;
	}
}


//Generates code and stores it at current code index
void emit(int op, int r, int l, int m)
{
	code[cx].op = op; //Store op in currect structs op
	code[cx].r = r; //Store r in currect structs r
	code[cx].l = l; //Store l in currect structs l
	code[cx].m = m; //Store m in currect structs m
	cx++;	//increment index
}
