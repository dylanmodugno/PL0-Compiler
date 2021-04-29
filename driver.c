#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "lex.c"
#include "parser.h"
#include "parser.c"
#include "codegen.h"
#include "codegen.c"
#include "vm.h"
#include "vm.c"

int main(int argc, char **argv)
{
	if (argc < 2) //If no input file
		printf("error : please include the file name\n");

	
	printf("Taking %s as input file\n", argv[1]); //Prints input file to console
	
	
	//Setting up flags
	int aFlag = 0, vFlag = 0, lFlag = 0;
		
	if (argc == 5)
	{
		aFlag = 1;
		vFlag = 1;
		lFlag = 1;
	}
	
	else if (argc != 2)
	{
		if (argv[2][1] == 'v')
    	    vFlag = 1;
    	else if (argv[2][1] == 'a')
    	    aFlag = 1;
    	else 
        	lFlag = 1;
        if (argc == 4)
        {
        	if (argv[3][1] == 'v')
            	vFlag = 1;
        	else if (argv[3][1] == 'a')
        	    aFlag = 1;
        	else 
        	    lFlag = 1;
    	}
	}
	
	lexeme *list = lex_analyze(argv[1]); //Create Lexeme List
	
	FILE* ofp = fopen("CompilerOutput.txt", "w+"); //Create back up output file
	
	int i = 0;
	
	//Print lexeme table
	if (lFlag == 1)
		printf("\nLexeme:\t\tToken Type:\n");
	fprintf(ofp,"Lexeme list:\n");
	
	while(list[i].type != 0)
	{
		if (lFlag == 1)
			printf("%s\t\t%d\n", list[i].ident, list[i].type);
		fprintf(ofp, "%s\t\t%d\n", list[i].ident, list[i].type);
		i++;
	}
	
	i = 0;
	
	//Print Lexeme list if lflag
	if (lFlag == 1)
		printf("\nLexeme list:\n");
	fprintf(ofp,"Lexeme list:\n");
	
	while(list[i].type != 0)
	{
		if (list[i].type == 2)
		{
			if (lFlag == 1)
				printf("%d %s ", list[i].type, list[i].ident);
			fprintf(ofp, "%d %s ", list[i].type, list[i].ident);
		}
		
		else if (list[i].type == 3)
		{
			if (lFlag == 1)
				printf("%d %d ", list[i].type, list[i].val);
			fprintf(ofp, "%d %d ", list[i].type, list[i].val);
		}
		
		else
		{
			if (lFlag == 1)
				printf("%d ", list[i].type);
			fprintf(ofp, "%d ", list[i].type);
		}
		i++;
	}
	
	if (lFlag == 1)
		printf("\n"); //Newline for reading
	fprintf(ofp,"\n");
	
	symbol *table = parse(list); //Call Parser
	
	Instruction *code = generate_code(table, list); //Call Code Generator
	
	i = 0;
	
	if (aFlag == 1) //If a flag is 1 print generated code
		printf("\nGenerated Code:\n");
	fprintf(ofp,"\nGenerated Code:\n");
	
	while(code[i].op != 0)
	{
		if (aFlag == 1)
			printf("%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);
		fprintf(ofp, "%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);
		i++;
	}
	
	virtual_machine(code, vFlag); //Call virtual machine, if v flag print stack
	
	return 0;
}
