#ifndef __LEX_H
#define __LEX_H

typedef enum { nulsym = 1, identsym, numbersym, plussym, minussym,multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
			gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,periodsym, becomessym, beginsym, endsym, ifsym, thensym,
			whilesym, dosym, callsym, constsym, varsym, procsym, writesym,readsym , elsesym } token_type;
			
typedef struct lexemes
{
	token_type type;
	int val;
	char* ident;
} lexeme;


lexeme* lex_analyze(char *inputfile);

#endif
