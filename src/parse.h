 /** @file
    Interface of parser.
 */

#ifndef PARSE_H
#define PARSE_H

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
 * @brief function for parsing commands
 * @param str the string that needs to be parsed
 * @return command structure that corresponds to the input, or null pointer if the input is incorrect
 */
command* parse_command(char *str);

#endif /* PARSE_H */
