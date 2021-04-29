
#include "lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_CHAR_LENGTH 1000
#define MAX_TEST_LENGTH 100

lexeme* lex_analyze(char *inputfile)
{	
	//Variables to handle file input
	char input[MAX_CHAR_LENGTH];
	char lengthTest[MAX_TEST_LENGTH];
	char temp;
	int i, charNum = 0;
	
	FILE* ifp = fopen(inputfile, "r");
	
    if (ifp == NULL) //If file not found
	{
        printf("Could not open file\n");
        exit(-1);
    }
	
	//Takes file input
	while(!feof(ifp))
	{
        temp = fgetc(ifp); 

        // checks if the scanned value is not EOF
        if(temp!=EOF)
        {
        	// puts scanned value in array
    		input[charNum] = temp;
    		
    		// update counter
        	charNum++;
		}
	}
	
	//Appends null terminator to end
	input[charNum] = '\0';
	
	//Print out the source program
	printf("Source Program:\n%s\n", input);
	
	//Replaces every newline/escape sequence/tab character with a space
	for (i = 0; i < charNum; i++)
	{
		if(input[i] == '\n' || input[i] == '\t' || input[i] == '\r' || input[i] == '\v')
		{
			input[i] = ' ';
		}
	}
		
	int progLength = strlen(input);
	
	//Tokenizing loop variables
	int inputIndex = 0, outputIndex = 0, scout = 0, numDigits = 0, containsAlpha = 0;
	lexeme* output = (lexeme*) malloc(500 * sizeof(lexeme));
	
	//Main loop
	while(inputIndex < progLength) //While current index is less than the length of the program
	{
		output[outputIndex].ident = malloc(12 * sizeof(char));
		if (isalpha(input[inputIndex])) //If begining of a token is an alpha char then it must be a keywork or an identifier
		{
			//Keyword begin
			if (input[inputIndex] == 'b' && input[inputIndex + 1] == 'e' && input[inputIndex + 2] == 'g' && input[inputIndex + 3] == 'i' && input[inputIndex + 4] == 'n' 
				&& !(isalpha(input[inputIndex + 5]) || isdigit(input[inputIndex + 5])))
			{
				output[outputIndex].type = beginsym;
				output[outputIndex].ident[0] = 'b';
				output[outputIndex].ident[1] = 'e';
				output[outputIndex].ident[2] = 'g';
				output[outputIndex].ident[3] = 'i';
				output[outputIndex].ident[4] = 'n';
				output[outputIndex].ident[5] = '\0';
				inputIndex += 5;
				outputIndex++;	
			}
			//Keyword call
			else if(input[inputIndex] == 'c' && input[inputIndex + 1] == 'a' && input[inputIndex + 2] == 'l' && input[inputIndex + 3] == 'l' && 
				!(isalpha(input[inputIndex + 4]) || isdigit(input[inputIndex + 4])))
			{
				output[outputIndex].type = callsym;
				output[outputIndex].ident[0] = 'c';
				output[outputIndex].ident[1] = 'a';
				output[outputIndex].ident[2] = 'l';
				output[outputIndex].ident[3] = 'l';
				output[outputIndex].ident[4] = '\0';
				inputIndex += 4;
				outputIndex++;
			}
			//Keyword const
			else if (input[inputIndex] == 'c' && input[inputIndex + 1] == 'o' && input[inputIndex + 2] == 'n' && input[inputIndex + 3] == 's' && input[inputIndex + 4] == 't' 
				&& !(isalpha(input[inputIndex + 5]) || isdigit(input[inputIndex + 5])))
			{
				output[outputIndex].type = constsym;
				output[outputIndex].ident[0] = 'c';
				output[outputIndex].ident[1] = 'o';
				output[outputIndex].ident[2] = 'n';
				output[outputIndex].ident[3] = 's';
				output[outputIndex].ident[4] = 't';
				output[outputIndex].ident[5] = '\0';
				inputIndex += 5;
				outputIndex++;	
			}
			//Keyword do
			else if(input[inputIndex] == 'd' && input[inputIndex + 1] == 'o' && !(isalpha(input[inputIndex + 2]) || isdigit(input[inputIndex + 2])))
			{
				output[outputIndex].type = dosym;
				output[outputIndex].ident[0] = 'd';
				output[outputIndex].ident[1] = 'o';
				output[outputIndex].ident[2] = '\0';
				inputIndex += 2;
				outputIndex++;
			}
			//Keyword else
			else if(input[inputIndex] == 'e' && input[inputIndex + 1] == 'l' && input[inputIndex + 2] == 's' && input[inputIndex + 3] == 'e' && 
				!(isalpha(input[inputIndex + 4]) || isdigit(input[inputIndex + 4])))
			{
				output[outputIndex].type = elsesym;
				output[outputIndex].ident[0] = 'e';
				output[outputIndex].ident[1] = 'l';
				output[outputIndex].ident[2] = 's';
				output[outputIndex].ident[3] = 'e';
				output[outputIndex].ident[4] = '\0';
				inputIndex += 4;
				outputIndex++;
			}
			//Keyword end
			else if(input[inputIndex] == 'e' && input[inputIndex + 1] == 'n' && input[inputIndex + 2] == 'd' && !(isalpha(input[inputIndex + 3]) || isdigit(input[inputIndex + 3])))
			{
				output[outputIndex].type = endsym;
				output[outputIndex].ident[0] = 'e';
				output[outputIndex].ident[1] = 'n';
				output[outputIndex].ident[2] = 'd';
				output[outputIndex].ident[3] = '\0';
				inputIndex += 3;
				outputIndex++;
			}
			//Keyword if
			else if(input[inputIndex] == 'i' && input[inputIndex + 1] == 'f' && !(isalpha(input[inputIndex + 2]) || isdigit(input[inputIndex + 2])))
			{
				output[outputIndex].type = ifsym;
				output[outputIndex].ident[0] = 'i';
				output[outputIndex].ident[1] = 'f';
				output[outputIndex].ident[2] = '\0';
				inputIndex += 2;
				outputIndex++;
			}	
			//Keyword odd
			else if(input[inputIndex] == 'o' && input[inputIndex + 1] == 'd' && input[inputIndex + 2] == 'd' && !(isalpha(input[inputIndex + 3]) || isdigit(input[inputIndex + 3])))
			{
				output[outputIndex].type = oddsym;
				output[outputIndex].ident[0] = 'o';
				output[outputIndex].ident[1] = 'd';
				output[outputIndex].ident[2] = 'd';
				output[outputIndex].ident[3] = '\0';
				inputIndex += 3;
				outputIndex++;
			}
			//Keyword procedure
			else if (input[inputIndex] == 'p' && input[inputIndex + 1] == 'r' && input[inputIndex + 2] == 'o' && input[inputIndex + 3] == 'c' && input[inputIndex + 4] == 'e' 
				&& input[inputIndex + 5] == 'd' && input[inputIndex + 6] == 'u' && input[inputIndex + 7] == 'r' && input[inputIndex + 8] == 'e'
				&& !(isalpha(input[inputIndex + 9]) || isdigit(input[inputIndex + 9])))
			{
				output[outputIndex].type = procsym;
				output[outputIndex].ident[0] = 'p';
				output[outputIndex].ident[1] = 'r';
				output[outputIndex].ident[2] = 'o';
				output[outputIndex].ident[3] = 'c';
				output[outputIndex].ident[4] = 'e';
				output[outputIndex].ident[5] = 'd';
				output[outputIndex].ident[6] = 'u';
				output[outputIndex].ident[7] = 'r';
				output[outputIndex].ident[8] = 'e';
				output[outputIndex].ident[9] = '\0';
				inputIndex += 9;
				outputIndex++;	
			}
			//Keyword read
			else if(input[inputIndex] == 'r' && input[inputIndex + 1] == 'e' && input[inputIndex + 2] == 'a' && input[inputIndex + 3] == 'd' && 
				!(isalpha(input[inputIndex + 4]) || isdigit(input[inputIndex + 4])))
			{
				output[outputIndex].type = readsym;
				output[outputIndex].ident[0] = 'r';
				output[outputIndex].ident[1] = 'e';
				output[outputIndex].ident[2] = 'a';
				output[outputIndex].ident[3] = 'd';
				output[outputIndex].ident[4] = '\0';
				inputIndex += 4;
				outputIndex++;
			}
			//Keyword then
			else if(input[inputIndex] == 't' && input[inputIndex + 1] == 'h' && input[inputIndex + 2] == 'e' && input[inputIndex + 3] == 'n' && 
				!(isalpha(input[inputIndex + 4]) || isdigit(input[inputIndex + 4])))
			{
				output[outputIndex].type = thensym;
				output[outputIndex].ident[0] = 't';
				output[outputIndex].ident[1] = 'h';
				output[outputIndex].ident[2] = 'e';
				output[outputIndex].ident[3] = 'n';
				output[outputIndex].ident[4] = '\0';
				inputIndex += 4;
				outputIndex++;
			}
			//Keyword var
			else if(input[inputIndex] == 'v' && input[inputIndex + 1] == 'a' && input[inputIndex + 2] == 'r' && !(isalpha(input[inputIndex + 3]) || isdigit(input[inputIndex + 3])))
			{
				output[outputIndex].type = varsym;
				output[outputIndex].ident[0] = 'v';
				output[outputIndex].ident[1] = 'a';
				output[outputIndex].ident[2] = 'r';
				output[outputIndex].ident[3] = '\0';
				inputIndex += 3;
				outputIndex++;
			}
			//Keyword while
			else if (input[inputIndex] == 'w' && input[inputIndex + 1] == 'h' && input[inputIndex + 2] == 'i' && input[inputIndex + 3] == 'l' && input[inputIndex + 4] == 'e' 
				&& !(isalpha(input[inputIndex + 5]) || isdigit(input[inputIndex + 5])))
			{
				output[outputIndex].type = whilesym;
				output[outputIndex].ident[0] = 'w';
				output[outputIndex].ident[1] = 'h';
				output[outputIndex].ident[2] = 'i';
				output[outputIndex].ident[3] = 'l';
				output[outputIndex].ident[4] = 'e';
				output[outputIndex].ident[5] = '\0';
				inputIndex += 5;
				outputIndex++;	
			}
			//Keyword write
			else if (input[inputIndex] == 'w' && input[inputIndex + 1] == 'r' && input[inputIndex + 2] == 'i' && input[inputIndex + 3] == 't' && input[inputIndex + 4] == 'e' 
				&& !(isalpha(input[inputIndex + 5]) || isdigit(input[inputIndex + 5])))
			{
				output[outputIndex].type = writesym;
				output[outputIndex].ident[0] = 'w';
				output[outputIndex].ident[1] = 'r';
				output[outputIndex].ident[2] = 'i';
				output[outputIndex].ident[3] = 't';
				output[outputIndex].ident[4] = 'e';
				output[outputIndex].ident[5] = '\0';
				inputIndex += 5;
				outputIndex++;	
			}
			else //If it's not a keyword then it's an identifier
			{
				scout = 0;
				//This token begins with a letter but may contain digits, this token can only end in a non digit non letter char
				while(isdigit(input[inputIndex]) || isalpha(input[inputIndex]))
				{
					//Store possible token in temp array and increment iterating variables 
					lengthTest[scout] = input[inputIndex];
					scout++;
					inputIndex++;
				}
				//If the identifier contains 11 or less characters, it is considered valid
				if (scout < 12)
				{
					strcpy(output[outputIndex].ident, lengthTest); 
					output[outputIndex].type = identsym;
					output[outputIndex].ident[scout] = '\0';
					outputIndex++;
				}
				//If the identifier contains more than 11 characters, it is considered an invalid identifier
				else
				{
					lengthTest[scout] = '\0';
					printf("\nError in Scanning: Variable %s exceeds max length of 11 characters\n", lengthTest);
					exit(0);
				}
			}			
		}
		else if (isdigit(input[inputIndex]))//Handles tokens that begin with a digit
		{
			scout = 0;
			numDigits = 0;
			containsAlpha = 0;
			//This token begins with a digit but may contain a letter, this token can only end in a non digit non letter char
			while(isdigit(input[inputIndex]) || isalpha(input[inputIndex]))
			{
				//If the token contains a letter
				if (isalpha(input[inputIndex]))
					containsAlpha++;
				else
					numDigits++;
					
				//Store possible token in temp array and increment iterating variables 
				lengthTest[scout] = input[inputIndex];
				scout++;
				inputIndex++;
			}
			//If the token starts with less than 6 digit but contains a letter it is considered an invalid variable
			if (containsAlpha > 0 && numDigits < 6)
			{
				lengthTest[scout] = '\0';
				printf("\nError in Scanning: Variable %s begins with non letter character\n", lengthTest);
				exit(0);
			}
			//If the number contains 5 or less digits, it is considered valid
			else if (scout < 6)
			{
				strcpy(output[outputIndex].ident, lengthTest); 
				output[outputIndex].type = numbersym;
				output[outputIndex].ident[scout] = '\0';
				output[outputIndex].val = atoi(output[outputIndex].ident);
				//output[outputIndex].val = 0;
				
				outputIndex++;
			}
			//If the number contains more than 5 digits, it is considered an invalid number
			else
			{
				lengthTest[scout] = '\0';
				printf("\nError in Scanning: Number %s exceeds max length of 5 digits\n", lengthTest);
				exit(0);
			}			
		}
		else //Handles special characters
		{
			//Relational Equals
			if(input[inputIndex] == '=')
			{
				output[outputIndex].type = eqsym;
				output[outputIndex].ident[0] = '=';
				output[outputIndex].ident[1] = '\0';
				inputIndex++;
				outputIndex++;			
			}
			//Rational Less Than
			else if(input[inputIndex] == '<')
			{
				//Rational Less Than or Equals
				if (input[inputIndex + 1] == '=')
				{
					output[outputIndex].type = leqsym;
					output[outputIndex].ident[0] = '<';
					output[outputIndex].ident[1] = '=';
					output[outputIndex].ident[2] = '\0';
					inputIndex += 2;
					outputIndex++;
				}
				//Rational Not Equals
				else if (input[inputIndex + 1] == '>')
				{
					output[outputIndex].type = neqsym;
					output[outputIndex].ident[0] = '<';
					output[outputIndex].ident[1] = '>';
					output[outputIndex].ident[2] = '\0';
					inputIndex += 2;
					outputIndex++;
				}
				//Rational Less Than
				else
				{
					output[outputIndex].type = lessym;
					output[outputIndex].ident[0] = '<';
					output[outputIndex].ident[1] = '\0';
					inputIndex++;
					outputIndex++;		
				}
			}
			else if(input[inputIndex] == '>') //If the token begins with >
			{
				//Rational Less Than or Equals
				if (input[inputIndex + 1] == '=')
				{
					output[outputIndex].type = geqsym;
					output[outputIndex].ident[0] = '>';
					output[outputIndex].ident[1] = '=';
					output[outputIndex].ident[2] = '\0';
					inputIndex += 2;
					outputIndex++;
				}
				//Rational Less Than
				else
				{
					output[outputIndex].type = gtrsym;
					output[outputIndex].ident[0] = '>';
					output[outputIndex].ident[1] = '\0';
					inputIndex++;
					outputIndex++;		
				}
			}
			//Assignment
			else if(input[inputIndex] == ':') //If the token begins with a :
			{
				//Maybe dont need if just else if
				if (input[inputIndex + 1] == '=')
				{
					output[outputIndex].type = becomessym;
					output[outputIndex].ident[0] = ':';
					output[outputIndex].ident[1] = '=';
					output[outputIndex].ident[2] = '\0';
					inputIndex += 2;
					outputIndex++;
				}
				else
				{
					printf("\nError in Scanning: Invalid Symbol: %c at character %d\n", input[inputIndex], inputIndex);
					exit(0);
					inputIndex++;
				}
			}
			//Plus symbol
			else if(input[inputIndex] == '+')
			{
					output[outputIndex].type = plussym;
					output[outputIndex].ident[0] = '+';
					output[outputIndex].ident[1] = '\0';
					inputIndex++;
					outputIndex++;	
			}
			//Minus symbol
			else if(input[inputIndex] == '-')
			{
					output[outputIndex].type = minussym;
					output[outputIndex].ident[0] = '-';
					output[outputIndex].ident[1] = '\0';
					inputIndex++;
					outputIndex++;	
			}
			//Multiplications symbol
			else if(input[inputIndex] == '*')
			{
				output[outputIndex].type = multsym;
				output[outputIndex].ident[0] = '*';
				output[outputIndex].ident[1] = '\0';
				inputIndex++;
				outputIndex++;					
			}
			//Forwardslash
			else if(input[inputIndex] == '/')
			{
				//Division
				if (input[inputIndex + 1] != '*')
				{
					output[outputIndex].type = slashsym;
					output[outputIndex].ident[0] = '/';
					output[outputIndex].ident[1] = '\0';
					inputIndex++;
					outputIndex++;	
				}
				//Comment
				else
				{
					scout = inputIndex + 2; 
					while((input[scout] != '*' || input[scout + 1] != '/') && scout <= progLength) // Look forward until comment ends or program ends
						scout++;
					if(scout > progLength)
						inputIndex = scout;
					else
						inputIndex = scout + 2;
				}
			}
			//Open Parenthesis
			else if(input[inputIndex] == '(')
			{
					output[outputIndex].type = lparentsym;
					output[outputIndex].ident[0] = '(';
					output[outputIndex].ident[1] = '\0';
					inputIndex++;
					outputIndex++;	
			}
			//Close Parenthesis
			else if(input[inputIndex] == ')')
			{
					output[outputIndex].type = rparentsym;
					output[outputIndex].ident[0] = ')';
					output[outputIndex].ident[1] = '\0';
					inputIndex++;
					outputIndex++;	
			}
			//Comma
			else if (input[inputIndex] == ',')
			{
				output[outputIndex].type = commasym;
				output[outputIndex].ident[0] = ',';
				output[outputIndex].ident[1] = '\0';
				inputIndex++;
				outputIndex++;
			}
			//Semicolon
			else if (input[inputIndex] == ';')
			{
				output[outputIndex].type = semicolonsym;
				output[outputIndex].ident[0] = ';';
				output[outputIndex].ident[1] = '\0';
				inputIndex++;
				outputIndex++;
			}
			//Period
			else if (input[inputIndex] == '.')
			{
				output[outputIndex].type = periodsym;
				output[outputIndex].ident[0] = '.';
				output[outputIndex].ident[1] = '\0';
				inputIndex++;
				outputIndex++;
			}
			else if (input[inputIndex] == ' ') // Ignore whitespace in this case spaces
				inputIndex++;
			else //If it's not a reognized special character output a warning
			{
				printf("\nWARNING: Invalid Symbol: %c at character %d\n", input[inputIndex], inputIndex);
				exit(0);
				inputIndex++;
			}
		}
	}	
	return output;
}
