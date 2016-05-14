 /** @file
    Interface of parser.

 */

#ifndef PARSE_H
#define PARSE_H

#define INIT_CODE 0
#define MOVE_CODE 1
#define PRODUCE_PEASANT_CODE 2
#define PRODUCE_KNIGHT_CODE 3
#define END_TURN_CODE 4

typedef struct def_command {
	int commandCode;
	int data[7];
} command;


/** Reads a command.
  Returns command with data points using 'command' structure.
  */
command* parse_command();

#endif /* PARSE_H */
