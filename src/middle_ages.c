#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "engine.h"

int main() {

	start_game();

	command *new_command;
	while (1) {
		new_command = parse_command();

		if (strcmp(new_command->name, "INIT") == 0) 
		{
			if (!init(new_command->data[0],
					new_command->data[1],
					new_command->data[2],
					new_command->data[3],
					new_command->data[4],
					new_command->data[5],
					new_command->data[6]))
			{
				fprintf(stderr, "input error\n");
				return 42;
			}
		}
		else if (strcmp(new_command->name, "MOVE") == 0) 
		{
			if (!move(new_command->data[0],
					new_command->data[1],
					new_command->data[2],
					new_command->data[3]))
			{

				fprintf(stderr, "input error\n");
				return 42;
			}	
		}
		else if (strcmp(new_command->name, "PRODUCE_KNIGHT") == 0)
		{
			if (!produce_knight(new_command->data[0],
					new_command->data[1],
					new_command->data[2],
					new_command->data[3]))
			{

				fprintf(stderr, "input error\n");
				return 42;
			}
		}
		else if (strcmp(new_command->name, "PRODUCE_PEASANT") == 0)
		{
			if (!produce_peasant(new_command->data[0],
					new_command->data[1],
					new_command->data[2],
					new_command->data[3]))
			{

				fprintf(stderr, "input error\n");
				return 42;
			}
		}
		else if (strcmp(new_command->name, "END_TURN") == 0)
		{
			end_turn();
		}
		else
		{
			fprintf(stderr, "input error\n");
			return 42;	
		}

		print_topleft();
		printf("\n");
	}

	end_game();

	return 0;
}
