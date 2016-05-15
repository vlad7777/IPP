/** @file
    Interface of game engine.
 */

#ifndef ENGINE_H
#define ENGINE_H

/**
 * @brief Initializes a game. Needed before first INIT.
 */
void start_game();

/**
 * @brief Frees memory. Needed after finishing game.
 */
int end_game();

/**
 * @brief Checks if the game is over
 * @return 1 if the game is over, 0 othervise
 */
int isGameOver();

/**
 * @brief Initializes a game with size of a board, number of rounds and positions of kings.
 * @param n size of the board
 * @param k max number of moves per player
 * @param x1 first player's king's x coordinate
 * @param y1 first player's king's y coordinate
 * @param x2 second player's king's x coordinate
 * @param y2 second player's king's y coordinate
 * @return 1 if no error encountered, 0 otherwise
 */
int init(int n, int k, int p, int x1, int y1, int x2, int y2);

/**
 * @brief Makes a move.
 * @param[in] x1 Column number before a move.
 * @param[in] y1 Row number before a move.
 * @param[in] x2 Column number after a move.
 * @param[in] y2 Row number before a move.
 * @return 1 if no error was encountered, 0 otherwise.
 */
int move(int x1, int y1, int x2, int y2);

/**
 * @brief produces knight
 * @param x1 x coordinate of the peasant
 * @param y1 y coordinate of the peasant
 * @param x2 x coordinate of where to produce knight
 * @param y2 y coordinate of where to produce knight
 * @return 1 if no error was encountered, 0 otherwise.
 */
int produce_knight(int x1, int y1, int x2, int y2);

/**
 * @brief produces peasant
 * @param x1 x coordinate of the peasant
 * @param y1 y coordinate of the peasant
 * @param x2 x coordinate of where to produce peasant 
 * @param y2 y coordinate of where to produce peasant 
 * @return 1 if no error was encountered, 0 otherwise.
 */
int produce_peasant(int x1, int y1, int x2, int y2);

/**
 * @brief ends current turn
 * @return 1 if no error was encountered, 0 otherwise
 */
int end_turn();

/**
 * Prints (into stdout) top-left corner of the board of size m x m where m = min(n,10).
 */
void print_topleft();

#endif /* ENGINE_H */
