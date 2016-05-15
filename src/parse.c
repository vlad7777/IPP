/** @file
 *  	implementation of parse
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define INIT_CODE 0 ///<code for init command
#define MOVE_CODE 1 ///<code for move command
#define PRODUCE_PEASANT_CODE 2 ///<code for produce peasant command
#define PRODUCE_KNIGHT_CODE 3 ///<code for produce knight command
#define END_TURN_CODE 4 ///<code for end turn command

/** 
 * structure for holding information about commands 
 */
typedef struct def_command {
	int commandCode; ///<stores command code
	int data[7]; ///< stores command parameters 
} command;

/**
 * an array that hold numbers of required arguments for each command.
 */
int arg_count[] = {7, 4, 4, 4, 0};

/**
 * function created solely for debuggin purposes
 * prints the given command to stdin
 */
void print_command(command *c) 
{
	if (c == 0)
		return;	
	printf("%d\n", c->commandCode);
	for (int i = 0; i < 7 && c->data[i] > -1; i++)
		printf("%d ", c->data[i]);
	printf("\n");
}

/**
 * @param str string that needs to be parsed
 * @param ind index that we change when going through the string
 * @return code of command contained in the string
 *
 * codes are: 
 * INIT 0
 * MOVE 1
 * PRODUCE_PEASANT 2
 * PRODUCE_KNIGHT 3
 * END_TURN 4
 */
int getCommandCode(char *str, int *ind)
{
	char c[20];	
	int i = 0;
	int l = strlen(str);
	while (str[*ind] != ' ' && str[*ind] != '\n' && *ind < l && i < 20)
	{
		c[i++] = str[*ind];
		(*ind)++;
	} 
	int *ar = (int*)malloc(sizeof(int) * 10);
	if (i >= 20)
		return -1;
	c[i] = '\0';

	if (strcmp(c, "INIT") == 0)
		return INIT_CODE;
	if (strcmp(c, "MOVE") == 0)
		return MOVE_CODE;
	if (strcmp(c, "PRODUCE_PEASANT") == 0)
		return PRODUCE_PEASANT_CODE;
	if (strcmp(c, "PRODUCE_KNIGHT") == 0)
		return PRODUCE_KNIGHT_CODE;
	if (strcmp(c, "END_TURN") == 0)
		return END_TURN_CODE;
	//puts("1");

	return -1;
}

/**
 * @param ind index where we start parsing 
 * @param str string that we parse
 * @return next integer after position pointed at by ind or -1 if 
 */
int readNextInd(char *str, int *ind)
{
	int ret = 0;
	int flag = 0;
	while (isdigit(str[*ind]))
	{
		ret *= 10;
		ret += (str[*ind] - '0');
		if (ret == 0 && str[*ind] == 0)
			return -1;
		flag = 1;
		(*ind)++;
	}
	if (flag == 0)
	{
		(*ind)++;
		return -1;
	}
	return ret;
}

command* parse_command(char *temp) {	
	if (strlen(temp) <= 1 || strlen(temp) >= 100)
		return 0;
	//puts(temp);

	command *ret = malloc(sizeof(command));
	int str_ind = 0;
	int l = strlen(temp);
	int data_ind = 0;
	int code = getCommandCode(temp, &str_ind);
	ret->commandCode = code;
	if (code == -1)
	{
		free(ret);
		return 0;
	}
	while (str_ind < l && temp[str_ind] == ' ')
	{
		str_ind++;
		int x = readNextInd(temp, &str_ind);
		if (x == -1)
		{
			free(ret);
			return 0;
		}
		ret->data[data_ind++] = x;
	}

	//printf("%d %d %d\n", data_ind, code, arg_count[code]);
	if (data_ind != arg_count[code])
	{
		free(ret);
		return 0;
	}
	//print_command(ret);
	return ret;
}
