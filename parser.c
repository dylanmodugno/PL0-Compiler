#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

lexeme* tokenList;
int tokenIndex;
symbol *table;
int tableLength = 0;

symbol* parse(lexeme *list)
{
	//symbol *table = malloc(500 * sizeof(symbol));
	table = malloc(500 * sizeof(symbol));
	tokenList = list;
	Program();
	printf("\nParsing complete. No Errors Detected\n");
	return table;
}

/*
PROGRAM
	add to symbol table (kind 3, "main", 0, lexlevel 0, m 0, unmarked)
	BLOCK (0)
	if token != .
		error
*/

void Program()
{
	addToSymbol(3, "main" , 0, 0, 0, 0);
	Block(0); // Call BLOCK
	if (tokenList[tokenIndex++].type != periodsym)
		Error(1);
}

		
/*
BLOCK
	numSymbols = 0;
	numSymbols += CONST-DECLARATION (lexlevel)
	numSymbols += VAR-DECLARATION (lexlevel)
	numSymbols += PROCEDURE-DECLARATION (lexlevel + 1)
	STATEMENT (lexlevel)
	mark the last numSymbols number of unmarked symbols
*/

void Block(int lexlevel)
{
	int numSymbols = 0;
	
	
	numSymbols += ConstDeclaration(lexlevel); // Call CONST-DECLARATION
	
	numSymbols += VarDeclaration(lexlevel); // Call DECLARATION
	
	numSymbols += ProcDeclaration(lexlevel + 1);
	
	Statement(lexlevel); // Call STATEMENT
	
	markLastNumSymbols(numSymbols);
}
	
/*	
returns numConsts CONST-DECLARATION (lexlevel)
	if token == const
		do
			get next token
			if token != ident
				error
			save ident name
			if the ident name is already in the symbol table unmarked and at the same lexlevel
				error
			get next token
			if token != =
				error
			get next token
			if token != number
				error
			add to the symbol table (kind 1, saved name, number is current token, lexlevel, m 0, unmarked)
			get next token
		while token == ,
		if token != ;
			error
		get next token
*/
	
int ConstDeclaration(int lexlevel)
{
	int numConsts = 0;
	char* tempName;
	int alreadyThere;
	if (tokenList[tokenIndex].type == constsym)
	{
		do
		{	
			tokenIndex++;		
			if (tokenList[tokenIndex].type != identsym)
				Error(2);
			tempName = tokenList[tokenIndex].ident;
			alreadyThere = checkForUnmarkedAndLex(lexlevel);
			if (alreadyThere == 1)
				Error(3);
			tokenIndex++;
			if (tokenList[tokenIndex].type != eqsym)
				Error(4);
			tokenIndex++;
			if (tokenList[tokenIndex].type != numbersym)
				Error(5);
			addToSymbol(1, tempName, tokenList[tokenIndex].val, 0, 0, 0);
			numConsts++; //idk if this is actual what to do
			tokenIndex++;	
		} while(tokenList[tokenIndex].type == commasym);
		if (tokenList[tokenIndex].type != semicolonsym)
			Error(6);
		tokenIndex++;
	}
	return numConsts;
}
		
/*		
returns numVars VAR-DECLARATION (lexlevel)
	if token == var
		numVars = 0
		do
			numVars++
			get next token
			if token != ident
				error
			if the ident name is already in the symbol table unmarked and at the same lexlevel
				error
			add to symbol table (kind 2, name is current token, val 0, lexlevel, m var# + 2, unmarked)
			get next token
		while token == ,
		if token != ;
			error
		get next token
*/

int VarDeclaration(int lexlevel)
{
	int numVars = 0;
	if (tokenList[tokenIndex].type == varsym)
	{
		int alreadyThere;
		do
		{
			numVars++;
			tokenIndex++;
			if (tokenList[tokenIndex].type != identsym)
				Error(7);
			alreadyThere = checkForUnmarkedAndLex(lexlevel);
			if (alreadyThere == 1)
				Error(8);
			addToSymbol(2, tokenList[tokenIndex].ident, 0, 0, numVars + 2, 0);
			tokenIndex++;
		}while (tokenList[tokenIndex].type == commasym);
		if (tokenList[tokenIndex].type != semicolonsym)
				Error(9);
		tokenIndex++;
	}
	return numVars;
}

/*
returns numProcedures PROCEDURE-DECLARATION (lexlevel)
	if token == procedure
		do
			get next token
			if token != ident
				error
			if ident is already in the symbol table unmarked and at the same level
				error
			add to the symbol table (kind 3, ident, val 0, lexlevel, m 0, unmarked)
			get next token
			if token != ;
				error
			get next token
			BLOCK(lexlevel)
			if token != ;
				error
			get next token
		while token == procedure
*/

int ProcDeclaration (int lexlevel)
{
	int numProcs = 0;
	if (tokenList[tokenIndex].type == procsym)
	{
		do	{		
			tokenIndex++;
			if (tokenList[tokenIndex].type != identsym)
				Error(7);
			if (checkForUnmarkedAndLex(lexlevel) == 1)
				Error(25);
			addToSymbol(3, tokenList[tokenIndex].ident, 0, lexlevel, 0, 0);
			numProcs++;
			tokenIndex++;
			if (tokenList[tokenIndex].type != semicolonsym)
				Error(24);
			tokenIndex++;
			Block(lexlevel);
			if (tokenList[tokenIndex].type != semicolonsym)
				Error(24);
			tokenIndex++;
		}while (tokenList[tokenIndex].type == procsym);
	}
	return numProcs;
}
		
/*		
STATEMENT (lexlevel)
	if token == ident
		search the symbol table working backwards find the latest unmarked var with the desired name
		if you can't find one
			error
		get next token
		if token != :=
			error
		get next token
		EXPRESSION (lexlevel)
		return
	if token == call
		get next token
		search the symbol table working backwards find the latest unmarked procedure with the desired 
		if you can't find one
			error
		get next token
		return
	if token == begin
		get next token
		STATEMENT (lexlevel)
		while token == ;
			get next token
			STATEMENT (lexlevel)
		if token != end
			error
		get next token
		return
	if token == if
		get next token
		CONDITION (lexlevel)
		if token != then
			error
		get next token
		STATEMENT (lexlevel)
		if token == else
			get next token
			STATEMENT (lexlevel)
		return
	if token == while
		get next token
		CONDITION (lexlevel)
		if token != do
			error
		get next token
		STATEMENT (lexlevel)
		return
	if token == read
		get next token
		search the symbol table working backwards to find the latest unmarked var with the desired name
		if you can't find one
			error
		get next token
		return
	if token == write
		get next token
		EXPRESSION (lexlevel)
		return
	return
*/

void Statement(int lexlevel)
{
	int alreadyThere;
	/*
	if token == ident
		search the symbol table working backwards find the latest unmarked var with the desired name
		if you can't find one
			error
		get next token
		if token != :=
			error
		get next token
		EXPRESSION (lexlevel)
		return
	*/
	if (tokenList[tokenIndex].type == identsym)
	{
		alreadyThere = reverseSearch();
		if (alreadyThere == 0)
			Error(10);
		/*
		if (SymbolType(tokenList[tokenIndex].ident) != 2) // Maybe just need to check for varsym, leave until it breaks
			Error(11);
		*/
		tokenIndex++;
		if (tokenList[tokenIndex].type != becomessym)
			Error(12);
		tokenIndex++;
		Expression(lexlevel);
		return;
	}
	/*
	if token == call
		get next token
		search the symbol table working backwards find the latest unmarked procedure with the desired 
		if you can't find one
			error
		get next token
		return
	*/
	if (tokenList[tokenIndex].type == callsym)
	{
		tokenIndex++;
		alreadyThere = reverseSearch();
		if (alreadyThere == 0)
			Error(10);
		tokenIndex++;
		return;
	}
	/*
	if token == begin
		get next token
		STATEMENT (lexlevel)
		while token == ;
			get next token
			STATEMENT (lexlevel)
		if token != end
			error
		get next token
		return
	*/
	if (tokenList[tokenIndex].type == beginsym)
	{
		tokenIndex++;
		Statement(lexlevel);
		while (tokenList[tokenIndex].type == semicolonsym)
		{
			tokenIndex++;
			Statement(lexlevel);
		}
		if (tokenList[tokenIndex].type != endsym)
			Error(13);
		tokenIndex++;
		return;	
	}
	/*
	if token == if
		get next token
		CONDITION (lexlevel)
		if token != then
			error
		get next token
		STATEMENT (lexlevel)
		if token == else
			get next token
			STATEMENT (lexlevel)
		return
	*/
	if (tokenList[tokenIndex].type == ifsym)
	{
		tokenIndex++;
		Condition(lexlevel);
		if (tokenList[tokenIndex].type != thensym)
			Error(14);
		tokenIndex++;
		Statement(lexlevel);
		if (tokenList[tokenIndex].type == elsesym)
		{
			tokenIndex++;
			Statement(lexlevel);
		}
		return;	
	}
	/*
	if token == while
		get next token
		CONDITION (lexlevel)
		if token != do
			error
		get next token
		STATEMENT (lexlevel)
		return
	*/
	if (tokenList[tokenIndex].type == whilesym)
	{
		tokenIndex++;
		Condition(lexlevel);
		if (tokenList[tokenIndex].type != dosym)
			Error(15);
		tokenIndex++;
		Statement(lexlevel);
		return;	
	}
	/*
	if token == read
		get next token
		search the symbol table working backwards to find the latest unmarked var with the desired name
		if you can't find one
			error
		get next token
		return
	*/
	if (tokenList[tokenIndex].type == readsym)
	{
		tokenIndex++;
		if (tokenList[tokenIndex].type != identsym)
			Error(16);
		alreadyThere = reverseSearch();
		if (alreadyThere == 0)
			Error(17);
		if (SymbolType(tokenList[tokenIndex].ident) != 2) // Maybe just need to check for varsym, leave until it breaks
			Error(18);
		tokenIndex++;
		return;	
	}
	/*
	if token == write
		get next token
		EXPRESSION (lexlevel)
		return
	return
	*/
	if (tokenList[tokenIndex].type == writesym)
	{
		tokenIndex++;
	Expression(lexlevel);
		return;
	}
	return;		
}

/*
CONDITION
	if token == odd
		get next token
		EXPRESSION
	else
		EXPRESSION
		if token != '=', '<>', '<', '<=', '>', '>=' // here do a switch or something it has to be obe of these or error
			error
		get next token
		EXPRESSION
*/

void Condition(int lexlevel)
{
	if (tokenList[tokenIndex].type == oddsym)
	{
		tokenIndex++;
		Expression(lexlevel);
	}
	else
	{
		Expression(lexlevel);
		//eqsym, neqsym, lessym, leqsym, gtrsym, geqsym,
		if (tokenList[tokenIndex].type != eqsym && tokenList[tokenIndex].type != neqsym && tokenList[tokenIndex].type != lessym && tokenList[tokenIndex].type != leqsym 
				&&	tokenList[tokenIndex].type != gtrsym && tokenList[tokenIndex].type != geqsym)
		{
			Error(19);
		}
		tokenIndex++;
		Expression(lexlevel);	
	}
}

/*
EXPRESSION
	if token == '+', '-'
		get next token
	TERM
	while token == '+', '-'
		get next token
		TERM
*/

void Expression(int lexlevel)
{
	if (tokenList[tokenIndex].type == plussym || tokenList[tokenIndex].type == minussym)
	{
		tokenIndex++;
	}
	Term(lexlevel);
	while (tokenList[tokenIndex].type == plussym || tokenList[tokenIndex].type == minussym)
	{
		tokenIndex++;
		Term(lexlevel);
	}
}

/*
TERM
	FACTOR
	while token == '*', '/'
		get next token
		FACTOR
*/

void Term(int lexlevel)
{
	Factor(lexlevel);
	while (tokenList[tokenIndex].type == multsym || tokenList[tokenIndex].type == slashsym)
	{
		tokenIndex++;
		Factor(lexlevel);
	}
}
		
/*
FACTOR (lexlevel)
	if token == ident
		search through the symbol table working backwards to find the latest unmarked var or const with the desired name
		if you can't find one
			error
		get next token
	else if token == number
		get next token
	else if token == (
		get next token
		EXPRESSION (lexlevel)
		if token != )
			error
		get next token
	else
		error
*/

void Factor(int lexlevel)
{
	int alreadyThere;
	if (tokenList[tokenIndex].type == identsym)
	{
		alreadyThere = reverseSearch();
		if (alreadyThere == 0)
			Error(20);
		tokenIndex++;
	}
	else if (tokenList[tokenIndex].type == numbersym)
		tokenIndex++;
	else if (tokenList[tokenIndex].type == lparentsym)
	{
		tokenIndex++;
		Expression(lexlevel);
		if (tokenList[tokenIndex].type != rparentsym)
			Error(21);
		tokenIndex++;		
	}
	else
		Error(23);
}

//Adds current token to the symbol table
void addToSymbol(int kind, char* name, int value, int l, int m, int mark)
{
	table[tableLength].kind = kind;
	table[tableLength].name = name;
	table[tableLength].val = value;
	table[tableLength].level = l;
	table[tableLength].addr = m;
	table[tableLength].mark = mark;
	//printf("\n%d %s %d %d %d %d Table Length: %d\n", table[tableLength].kind, table[tableLength].name,
	//			 table[tableLength].val, table[tableLength].level, table[tableLength].addr, table[tableLength].mark, tableLength + 1);
	tableLength++;
}

//Checks if the current token already exists in the symbol table
int checkFor()
{
	int i = 0;
	if(tableLength == 0)
		return 0;
	else
	{
		while(i < tableLength)
		{
			if (strcmp (table[i].name, tokenList[tokenIndex].ident) == 0)
				return 1;
			i++;
		}
	return 0;
	}
}

//Checks if the current token already exists in the symbol table
int checkForUnmarkedAndLex(int lexlevel)
{
	int i = 0;
	if(tableLength == 0)
		return 0;
	else
	{
		while(i < tableLength)
		{
			if (strcmp (table[i].name, tokenList[tokenIndex].ident) == 0 && table[i].level == lexlevel && table[i].mark == 0)
				return 1;
			i++;
		}
	return 0;
	}
}

//Search symbol table backwords to find ident
//Returns 0 if not found returns 1 if found
int reverseSearch()
{
	//printf("\nReverse was ran looking for %s\n", tokenList[tokenIndex].ident);
	int i = tableLength - 1;
	if(tableLength == 0)
		return 0;
	else
	{
		while(i >= 0)
		{
			if (strcmp (table[i].name, tokenList[tokenIndex].ident) == 0 && table[i].mark == 0)
				return 1;
			i--;
		}
	return 0;
	}
}

void markLastNumSymbols(int numSymbols)
{
	int i = tableLength - 1;
	int lastIndex = tableLength - numSymbols;
	if(tableLength == 0)
		return;
	else
	{
		while(i >= lastIndex)
		{
			table[i].mark = 1;
			i--;
		}
	return;
	}
}

//Gets the type of a symbol given the symbols name
int SymbolType(char* name)
{
	int i = 0;
	while(i < tableLength)
	{
		if (strcmp (table[i].name, name) == 0)
			return table[i].kind;
		i++;
	}
	return 0;
}

//Error List
void Error (int code)
{
	switch(code)
	{
		case 1:
		{
			printf("\nError Type 1 - Missing a period at the end of the program\n");
			exit(1);
			break;
		}
		case 2:
		{
			printf("\nError Type 2 - Constant not followed by an identifier\n");
			exit(2);
			break;
		}
		case 3:
		{
			printf("\nError Type 3 - Constants %s already exists in symbol table\n", tokenList[tokenIndex].ident);
			exit(3);
			break;
		}
		case 4:
		{
			printf("\nError Type 4 - No = after a constant\n");
			exit(4);
			break;
		}
		case 5:
		{
			printf("\nError Type 5 - No number following assignment\n");
			exit(5);
			break;
		}
		case 6:
		{
			printf("\nError Type 6 - Constant declaration not followed by a semicolon\n");
			exit(6);
			break;
		}
		case 7:
		{
			printf("\nError Type 7 - Variable declaration not followed by an identifier\n");
			exit(7);
			break;
		}
		case 8:
		{
			printf("\nError Type 8 - Variable %s already exists in symbol table\n", tokenList[tokenIndex].ident);
			exit(8);
			break;
		}
		case 9:
		{
			printf("\nError Type 9 - Variable declaration not followed by a semicolon\n");
			exit(9);
			break;
		}
		case 10:
		{
			printf("\nError Type 10 - Identifier not in symbol table when expected to be in symbol table\n");
			exit(10);
			break;
		}
		case 11:
		{
			printf("\nError Type 11 - Assigning a value to %s which is a non variable\n", tokenList[tokenIndex].ident);
			exit(11);
			break;
		}
		case 12:
		{
			printf("\nError Type 12 - Did not encounter expected := operator\n");
			exit(12);
			break;
		}
		case 13:
		{
			printf("\nError Type 13 - No end after begin\n");
			exit(13);
			break;
		}
		case 14:
		{
			printf("\nError Type 14 - If not followed by a then\n");
			exit(14);
			break;
		}
		case 15:
		{
			printf("\nError Type 15 - While not followed by a do\n");
			exit(15);
			break;
		}
		case 16:
		{
			printf("\nError Type 16 - Read not followed by an identifier\n");
			exit(16);
			break;
		}
		case 17:
		{
			printf("\nError Type 17 - Identifier for read or write not in symbol table\n");
			exit(17);
			break;
		}
		case 18:
		{
			printf("\nError Type 18 - Identifier for read is not a variable\n");
			exit(18);
			break;
		}
		case 19:
		{
			printf("\nError Type 19 - Did not encounter the expected relational operator\n");
			exit(19);
			break;
		}
		case 20:
		{
			printf("\nError Type 20 - Identifier for Factor is not in symbol table\n");
			exit(20);
			break;
		}
		case 21:
		{
			printf("\nError Type 21 - No closing parenthesis after an open parenthesis\n");
			exit(21);
			break;
		}
		case 22:
		{
			printf("\nError Type 22 - Write not followed by an identifier\n");
			exit(22);
			break;
		}
		case 23:
		{
			printf("\nError Type 23 - Unexpected token in factor\n");
			exit(23);
			break;
		}
		case 24:
		{
			printf("\nError Type 24 - Procedure declaration not followed by a semicolon\n");
			exit(24);
			break;
		}
		case 25:
		{
			printf("\nError Type 25 - Procedure already exists in the symbol table with same lex level\n");
			exit(25);
			break;
		}
		//This should never run
		default:
		{
			printf("\nUknown Error\n");
			exit(0);
		}
	}
}
