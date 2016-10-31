/*
* tokenizer.c
* Andy Lim
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

#include "utils.h"

struct TokenizerT_ {
	char *token;
	char *current;
};

typedef struct TokenizerT_ TokenizerT;


TokenizerT *TKCreate(char * ts) {
	//if the string length is NULL stop there is nothing to do
	if (strlen(ts) == 0) 
		return NULL;

	//init new token
	TokenizerT *tk;
	tk = malloc(sizeof(TokenizerT));

	//copy it
	tk->token = malloc(strlen(ts) + 1);
	strcpy(tk->token, ts);

	//intilize current
	tk->current = tk->token;

	return tk;
}


void TKDestroy(TokenizerT * tk) {
	free(tk->token);
	free(tk);
}


char *TKGetNextToken(TokenizerT * tk) {
	//new pointer to current token and length for NULL ending
	char *place = tk->current;
	int tokenLength = 0;
	
	//start walking through the input string and splitting
	while (*place != '\0') {
		//if there is a space
		if (isspace(*place)) {
			
			//copy chars
			char *token = malloc(tokenLength + 1);
			strncpy(token, tk->current, tokenLength);
			token[tokenLength] = '\0';

			//move past to the next token
			tk->current = place;
			while (isspace(*tk->current)) 
				tk->current++;
			
			return token;
		}
		else {
			//move past non white space
			place++;
			tokenLength++;
		}
	}

	//end of inputstring
	char *token = malloc(tokenLength);
	strncpy(token, tk->current, tokenLength);
	token[tokenLength] = '\0';
	tk->current = place;

	return token;
}


int zero(char *s){
	char *place = s;
	//checks it see if its a zero
	if (*place == '0'){	
		//inc one to see if next char is zero
		place++;
		if (*place == '\0')
			//its a lone zero
			return true;
		//its something else
		else
			return false;
	}
	return false;
}
/*
* This function will check if the token is a float
*/
int floatNum(char *s) {
	//convert char using strtod
	char *place = s;
	strtod(place, &s);
	errno = 0;
	//check if is alphabetic 
	if(isalpha(*place)){
		return false;
	}
	//this checks to make sure its not an int value
	while (*place != '\0'){
		place++;
		//check for e E or .
		if (*place == 'e' || *place == 'E' || *place == '.')
			 return true;
	}
	//check if conversion fail '0' or '-' char
	if (errno || *place == '0' || *place == '-') {
		return false;
	}
	
	return false;
}

/*
* This function checks for a octal value
*/
int octal(char *s) {
	char *place = s;
	unsigned int x = *s;
	unsigned char c = x;

	//Check that first character is a zero
	if (*place != '0') {
		return false;
	}

	//start checking for octal
	while (*place != '\0'){
		//if not start printing non valid hex values
		if (!isdigit(*place)) {
			while (!isdigit(*place) && !iscntrl(*place)){
				x = *place;
				c = x;
				printf("Invalid: '%c' = [0x%02x]\n", c, x);
				place++;
			}
		}
		//if yes start printing octal 
		else if(*place == '0' && isdigit(*place) && *place != '8' && *place != '9'){
			if (*place == '0' && *place++ != '0'){	
				//inc one to see if next char is zero
				while (*place == '0' && *place != '\0'){
					//its zero
					printf("Zero\n");
					place++;
				}
			}
			else {
				place--;
				printf("Octal: ");
				while (isdigit(*place) && *place != '8' && *place != '9'){
					x = *place;
					c = x;
					printf("%c", c);
					place++;
				}
			printf("\n");
			}
		} 
		//if not start printing decimal values
		else if(isdigit(place)){
			printf("Decimal: ");
			while (isdigit(*place)){
				x = *place;
				c = x;
				printf("%c", c);
				place++;
			}
			printf("\n");
		}
	}
	//String contained non-octal values
	return false;
}
/*
* This function checks for a decimal value
*/
int decimal(char *s) {
	char *place = s;
	unsigned int x = *s;
	unsigned char c = x;
	strtod(place, &s);
	errno = 0;

	//if the token begins with a 0 it is not a decimal value
	if (*place == '0') {	
		return false;
	}
	//check if char is not digit or starts with 0
	while (*place != '\0') {
		//if not start printing non valid hex values
		if (!isdigit(*place)) {
			while (!isdigit(*place) && *place !='\0' ){
				x = *place;
				c = x;
				printf("Invalid: '%c' = [0x%02x]\n", c, x);
				place++;
			}
		}
		//if yes start printing octal values
		else if(isdigit(*place)){
			if (*place == '0' && *place != '0'){	
				//inc one to see if next char is zero
				while (*place == '0' && *place != '\0'){
					//its zero
					printf("Zero\n");
					place++;
				}
			}
			else if (*place == '0'){
				printf("Octal: ");
				while (isdigit(*place) && *place != '8' && *place != '9'){
					x = *place;
					c = x;
					printf("%c", c);
					place++;
				}
				printf("\n");
			}

			//if not decimal print decimal
			else { 
				printf("Decimal: ");
				while (isdigit(*place)){
					x = *place;
					c = x;
					printf("%c", c);
					place++;
				}
				printf("\n");
			}
		}
	}
	return true;
}

/*
* This function checks for a hex value
*/
int hex(char *s) {
	char *place = s;

	//check for a 0 and x and X
	if (*place++ == '0' && (*place == 'x' || *place == 'X')) {
		//check that chars are hex
		while (*(place++) != '\0') {
			if (isxdigit(*place)) {
				return true;
			}
		}
		return false;
	}
	return false;
}
/*
 * tests for escape characters
*/
char escapeCharacter(char *s) {
	char *place = s;
	//increment to get to the char past
	s++;
	char* hexValue = "";
	//don't allow char* longer than two chars
	if (strlen(place) > 2) 
		return false;
	
	//switch the char for it's hex value
	switch (*s) {
		case 'n': hexValue = "[0x0a]"; break;
		case 't': hexValue = "[0x09]"; break;
		case 'v': hexValue = "[0x0b]"; break;
		case 'b': hexValue = "[0x08]"; break;
		case 'r': hexValue = "[0x0d]"; break;
		case 'f': hexValue = "[0x0c]"; break;
		case 'a': hexValue = "[0x07]"; break;
		case '\\': hexValue = "[0x5c]"; break;
		case '\?': hexValue = "[0x3f]"; break;
		default: return false;
	}
	//print it
	printf("Escape character: '\\%s' = '%s'\n", s, hexValue);

	return true;
}
/*
* This function will compare token to the four values I was asked to check for. If it is not one of the five it will print Invalid.
*/
void printToken(char *tk) {
	//zero
	if (zero(tk)) 
		printf("Zero: '%s'\n", tk);
	else if (escapeCharacter(tk));
	//hex
	else if (hex(tk)) 
		printf("Hex: '%s'\n", tk);
	//float
	else if (floatNum(tk)) 
		printf("Float: '%s'\n", tk);
	//octal
	else if (octal(tk));
	//decimal
	else if (decimal(tk));
	//clean up
	free(tk);
}
/*
* main will have a string argument (in argv[1]).
* The string argument contains the tokens.
* Print out the tokens in the second string in left-to-right order.
* Each token should be printed on a separate line.
*/

int main(int argc, char **argv) {
	TokenizerT *tk;

	//check if there are enough args
	if (argc != 2) {
		printf("Invalid amount of arguments.\n");
		return 1;
	}

	//initliaze the tokenizer
	tk = TKCreate(argv[1]);

	//print new tokens
	printf("Tokens:\n%s\n", tk->token);
	char *token;

	//print new tokens and what they are //TODO
	while (*(token = TKGetNextToken(tk)) != '\0') 
		printToken(token);
	

	//clean up
	TKDestroy(tk);

	return 0;
}
