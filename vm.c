#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

int getInstructionLength(Instruction* list)
{
	int i = 0;
	while(list[i].op != 0)
		i++;
	return i;
}

void virtual_machine(Instruction *code, int vFlag)
{
	if (vFlag == 1)
		printf("\nStarting Virutal Machine\n");
	
	Instruction* instructionSet = code;
	
	int stack[1000];
	int regFile[8];
	int i = 0;
	char barStack[1000];
	
	//Initialize Stack to all zeroes
	for(i = 0; i < 1000; i++)
		stack[i] = 0;

	//Initialize Register File to all zeroes
	for(i = 0; i < 8; i++)
			regFile[i] = 0;	
	
	//Initialize Stack used for Activation Record Dividers
	for(i = 0; i < 1000; i++)
		barStack[i] = ' ';
	
	//Create and initialize Stack Pointer, Base Pointer, and Program Counter
	int SP = 1000;
	int BP = SP -1; 
	int PC = 0;
	
	int numInstruct = getInstructionLength(code);
	
	if (vFlag == 1)
	{
			//Prints to screen all the lines to be run
	    for (i = 0; i < numInstruct; i ++)
		{
			switch(instructionSet[i].op)
				{
					case 1: //Load literal
						printf("\n%d\tLIT\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 2: //Return
						printf("\n%d\tRTN\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;	
					case 3: //Load from stack
						printf("\n%d\tLOD\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 4: //Store from syack
						printf("\n%d\tSTO\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 5: //Call
						printf("\n%d\tCAL\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 6: //Increment Stack Pointer
						printf("\n%d\tINC\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 7: //Jump
						printf("\n%d\tJMP\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 8: //Jump Conditional
						printf("\n%d\tJPC\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 9: //System
						printf("\n%d\tSYS\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 10: //Negative
						printf("\n%d\tNEG\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 11: //Add
						printf("\n%d\tADD\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 12: //Subtract
						printf("\n%d\tSUB\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 13: //Multiply
						printf("\n%d\tMUL\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 14: //Divide
						printf("\n%d\tDIV\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 15: //Odd
						printf("\n%d\tODD\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 16: //Modulus
						printf("\n%d\tMOD\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 17: //Equals
						printf("\n%d\tEQL\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 18: //Not Equals
						printf("\n%d\tNEQ\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 19: //Less Than
						printf("\n%d\tLSS\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 20: //Less Than or Equal To
						printf("\n%d\tLEQ\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 21: //Greater Than
						printf("\n%d\tGTR\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
					case 22: //Greater Than or Equal To
						printf("\n%d\tGEQ\t%d\t%d\t%d\t", i, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
						break;
				}	
		}
		
		//Show current status of PC, BP, and SP
		printf("\n\n\t\t\t\tPC:\tBP:\tSP:\n");
		
		//Show current status of the stack and register file
		printf("Initial Values\t\t\t%d\t%d\t%d\n", PC, BP, SP);
		printf("Registers: ");
		
		for(i = 0; i < 8; i++)
			printf("%d ", regFile[i]);
		
		printf("\nStack: ");
		for(i = 970; i < SP; i++)
		printf("%d ", stack[i]);
	}
	
	
	
	//Variables for main loop
	int halt = 1;
	Instruction IR; //Create Instruction struct
	int prevPC; //Used only to track line number
	int base, lexLev; //Used to calculate lexicographical level
	
	//Main loop
	while(halt)
	{
		//Updates Line Number
		prevPC = PC;
		
		//Fills instruction struct with new instruction set
		IR.op = instructionSet[PC].op;
		IR.r = instructionSet[PC].r;
		IR.l = instructionSet[PC].l;
		IR.m = instructionSet[PC].m;
		PC++; //Updates PC
		
		switch(IR.op)
		{
			case 1: //Load literal
				{
					regFile[IR.r] = IR.m;
				}
				break;
			case 2: //Return
				{
					SP = BP + 1;
					barStack[SP - 1] = ' ';
					BP = stack[SP - 2];
					PC = stack[SP - 3];
				}
				break;	
			case 3: //Load from stack
				{
					//Calculates lexicographical level
					lexLev = IR.l;
					base = BP;
					while (lexLev > 0)  
					{
					    base = stack[base];
					    lexLev--;
					}
					
					regFile[IR.r] = stack[base - IR.m];
				}		
				break;
			case 4: //Store from stack
				{
					//Calculates lexicographical level
					lexLev = IR.l;
					base = BP;
					while (lexLev > 0)  
					{
					    base = stack[base];
					    lexLev--;
					}
					
					stack[base - IR.m] = regFile[IR.r];
				}
				break;
			case 5: //Call
				{
					//Calculates lexicographical level
					lexLev = IR.l;
					base = BP;
					while (lexLev > 0)  
					{
					    base = stack[base];
					    lexLev--;
					}
					
					stack[SP - 1] = base;
					barStack[SP - 1] = '|';
					stack[SP - 2] = BP;
					stack[SP - 3] = PC;
					BP = SP - 1;
					PC = IR.m;
				}	
				break;
			case 6: //Increment Stack Pointer
				{
					SP = SP - IR.m;
				}	
				break;
			case 7: //Jump
				{
					PC = IR.m;
				}	
				break;
			case 8: //Jump Conditional
				{
					if(regFile[IR.r] == 0)
						PC = IR.m;
				}
				break;
			case 9: //System
				{
					switch(IR.m)
					{
						//If output to screen
						case 1:
							printf("\n\nRegister %d: %d", IR.r, regFile[IR.r]);
							break;
						//If input from keyboard
						case 2:
							printf("\n\nPlease input an integer: ");
							scanf("%d", &regFile[IR.r]);
							break;
						//If program ends
						case 3:
							halt = 0;
					}
				}	
				break;
			case 10: //Negative
				{
					regFile[IR.r] = -1 * regFile[IR.r];
				}
				break;
			case 11: //Add
				{
					regFile[IR.r] = regFile[IR.l] + regFile[IR.m];
				}
				break;
			case 12: //Subtract
				{
					regFile[IR.r] = regFile[IR.l] - regFile[IR.m];
				}
				break;
			case 13: //Multiply
				{
					regFile[IR.r] = regFile[IR.l] * regFile[IR.m];
				}
				break;
			case 14: //Divide
				{
					regFile[IR.r] = regFile[IR.l] / regFile[IR.m];
				}
				break;
			case 15: //Odd
				{
					regFile[IR.r] = regFile[IR.r] % 2;
				}
				break;
			case 16: //Mod
				{
					regFile[IR.r] = regFile[IR.l] % regFile[IR.m];
				}
				break;
			case 17: //Equals
				{
					regFile[IR.r] = regFile[IR.l] == regFile[IR.m];
				}
				break;
			case 18: //Not Equals
				{
					regFile[IR.r] = regFile[IR.l] != regFile[IR.m];
				}
				break;
			case 19: //Less Than
				{
					regFile[IR.r] = regFile[IR.l] < regFile[IR.m];
				}
				break;
			case 20: //Less Than or Equal To
				{
					regFile[IR.r] = regFile[IR.l] <= regFile[IR.m];
				}
				break;
			case 21: //Greater Than
				{
					regFile[IR.r] = regFile[IR.l] > regFile[IR.m];
				}
				break;
			case 22: //Greater Than or Equal To
				{
					regFile[IR.r] = regFile[IR.l] >= regFile[IR.m];
				}
				break;
		}
		
		if (vFlag == 1)
		{
			//Show current status of PC, BP, and SP
			printf("\n\n\t\t\t\tPC:\tBP:\tSP:\n");
			
			//Show current line and current status of the stack and register file
			switch(IR.op)
			{
				case 1: //Load literal
					printf("%d LIT %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 2: //Return
					printf("%d RTN %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;	
				case 3: //Load from stack
					printf("%d LOD %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 4: //Store from syack
					printf("%d STO %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 5: //Call
					printf("%d CAL %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 6: //Increment Stack Pointer
					printf("%d INC %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 7: //Jump
					printf("%d JMP %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 8: //Jump Conditional
					printf("%d JPC %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 9: //System
					printf("%d SYS %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 10: //Negative
					printf("%d NEG %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 11: //Add
					printf("%d ADD %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 12: //Subtract
					printf("%d SUB %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 13: //Multiply
					printf("%d MUL %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 14: //Divide
					printf("%d DIV %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 15: //Odd
					printf("%d ODD %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 16: //Modulus
					printf("%d MOD %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 17: //Equals
					printf("%d EQL %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 18: //Not Equals
					printf("%d NEQ %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 19: //Less Than
					printf("%d LSS %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 20: //Less Than or Equal To
					printf("%d LEQ %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 21: //Greater Than
					printf("%d GTR %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
				case 22: //Greater Than or Equal To
					printf("%d GEQ %d %d %d\t\t\t%d\t%d\t%d\n", prevPC, IR.r, IR.l, IR.m, PC, BP, SP);
					break;
			}
		
			//Prints out the what is currerntly in the registers
			printf("Registers: ");
			for(i = 0; i < 8; i++)
				printf("%d ", regFile[i]);
		
			//Prints out the current stack
			printf("\nStack: ");
			for(i = 999; i >= SP; i--)
			{
				if(i == 0) //Prevent index out of bounds
					printf("%d ", stack[i]);
				
				else if(i == SP && IR.op == 5)//Prevents | from erroneoulsy being printed during the CAL execution
					printf("%d ", stack[i]);
				
				else //Prints stack value and | if there is an activation record
					printf("%d%c", stack[i],barStack[i-1]);
			}	
		}
		
		
	}
	
	printf("\n");
	return;
}
