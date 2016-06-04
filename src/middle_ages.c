#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "engine.h"

#define STR_SIZE 1000000

char temp[STR_SIZE];

int main() {

	start_game();

	command *new_command;

	while (!isGameOver())
	{
		if (!isMyTurn() && fgets(temp, STR_SIZE, stdin)) 
		{
			new_command = parse_command(temp);
			if (new_command == 0)
			{
				end_game();
				fprintf(stderr, "input error\n");
				return 42;
			}

			if (new_command->commandCode == INIT_CODE) 
			{
				if (!init(new_command->data[0],
							new_command->data[1],
							new_command->data[2],
							new_command->data[3],
							new_command->data[4],
							new_command->data[5],
							new_command->data[6]))
				{
					free(new_command);
					end_game();
					fprintf(stderr, "input error\n");
					return 42;
				}
				//print_topleft();
			}
			else if (new_command->commandCode == MOVE_CODE)
			{
				if (!move(new_command->data[0],
							new_command->data[1],
							new_command->data[2],
							new_command->data[3]))
				{
					free(new_command);
					end_game();
					fprintf(stderr, "input error\n");
					return 42;
				}	
				//print_topleft();
			}
			else if (new_command->commandCode == PRODUCE_KNIGHT_CODE)
			{
				if (!produce_knight(new_command->data[0],
							new_command->data[1],
							new_command->data[2],
							new_command->data[3]))
				{
					free(new_command);
					end_game();
					fprintf(stderr, "input error\n");
					//fprintf(stderr, "procude knight error");
					return 42;
				}
				//print_topleft();
			}
			else if (new_command->commandCode == PRODUCE_PEASANT_CODE)
			{
				if (!produce_peasant(new_command->data[0],
							new_command->data[1],
							new_command->data[2],
							new_command->data[3]))
				{
					free(new_command);
					end_game();
					fprintf(stderr, "input error\n");
					//fprintf(stderr, "procude peasant error");
					return 42;
				}
				//print_topleft();
			}
			else if (new_command->commandCode == END_TURN_CODE)
			{
				if (!end_turn())
				{
					free(new_command);
					fprintf(stderr, "input error\n");
					//fprintf(stderr, "end turn\n");
					return 42;
				}
			}
			else
			{
				free(new_command);
				end_game();
				fprintf(stderr, "input error\n");
				return 42;	
			}
			free(new_command);
		}
		else if (!isGameOver() && isMyTurn())
		{
			puts("END_TURN");
			end_turn();
			fflush(stdout);
			fflush(stderr);
		}
	}

	int res = end_game();
	if (res == 1)
	{
		return 1;
	}
	else
	{
		return res;
	}

	return 0;
}
