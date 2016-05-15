/** @file
   	implementation of the engine 
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define KNIGHT_CODE 0 ///< code that corresponds to knight piece
#define KING_CODE 1 ///< code that corresponds to king piece
#define PEASANT_CODE 2 ///< code that corresponds to peasant piece

#define HASH_BUCKETS_NUM 1003 ///< number of buckets used for hashing
#define HASH_QUOT 23LL ///< hashing quotient

/** computes maximum of two integers */
int max(int a, int b)
{
	if (a < b)
		return b;
	else
		return a;
}

/** computes minimum of two integers */
int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

/** enum that serves for holding information about piece types */
typedef enum def_PieceType 
{
	knight = KNIGHT_CODE,
	king = KING_CODE,
	peasant = PEASANT_CODE 
} PieceType;

/** array that holds fight outcomes for every pair of pieces **/
int fightResults[3][3] = {
	{0, 1, 1},
	{-1, 0, 1},
	{-1, -1, 0} };

/** structure that holds information about a piece **/
typedef struct def_Piece 
{
	PieceType type;///< type of the piece
	int player;///< player the piece belongs to 
	int x;///< x coordinates of the piece
	int y;///< y coordinate of the piece
	int lastMove;///< last time the piece was used
} Piece;

/** pieces list structure */
typedef struct def_PiecesList {
	Piece *current; ///< a pointer to the current element
	struct def_PiecesList *next; ///< a pointer to the next element of the list
} PiecesList;

/** strcuture for holding information about the current state of the game */
typedef struct def_Board {
	int size; ///<length of one side of the square board
	int maxMoves; ///<max amount of moves allowed per person
	PiecesList **hashBuckets; ///<hash buckets array for holding information about pieces
	int currentMove; ///< number of the current move
	int currentPlayer;///< number of the current player
	int isFirstPlayerInitialized; ///< 1 if INIT was called for the first player, 0 otherwise
	int isSecondPlayerInitialized; ///< 1 if INIT was called for the second player, 0 otherwise
	int isFirstKingDead; ///< 1 if the first king was killed, 0 otherwise
	int isSecondKingDead; ///< 1 if the second king was killed, 0 otherwise

} Board;

Board *board = 0; ///< global variable, the board that we play on

/** function for constructing an empty board 
 * @return empty board
 */
Board* constructBoard()
{
	Board *ret = malloc(sizeof(Board));	
	ret->size = -1;
	ret->hashBuckets = calloc(HASH_BUCKETS_NUM, sizeof(PiecesList*));
	ret->currentMove = 1;
	ret->currentPlayer = 1;
	ret->isSecondKingDead = 0;
	ret->isFirstKingDead = 0;
	ret->isSecondPlayerInitialized = 0;
	ret->isFirstPlayerInitialized = 0;
	return ret;
}

/**
 * @param x x coordinate of the piece we want to construct
 * @param y y coordinate of the piece we want to construct
 * @param player the player the constructed piece must belong to
 * @param type type of the piece we want to construct
 * @param lastMove the last time the constructed piece was used 
 * @return constructed piece 
 * */
Piece *constructPiece(int x, int y, int player, PieceType type, int lastMove)
{
	Piece *ret = malloc(sizeof(Piece));
	ret->x = x;
	ret->y = y;
	ret->player = player;
	ret->type = type;
	ret->lastMove = lastMove; 
	return ret;
}

/**
 * function that destructs a piece
 * @param piece the piece we want to destruct
 */
void destructPiece(Piece *piece)
{
	if (piece->type == king)
	{
		if (piece->player == 1)
			board->isFirstKingDead = 1;
		else 
			board->isSecondKingDead = 1;
	}	
	free(piece);
}

/**
 * a function that destructs a list
 * @param list list to be destructed
 */
void destructList(PiecesList *list)
{
	if (list == 0)
		return;
	destructList(list->next);
	free(list->current);
	free(list);
}

/**
 * a function used to destruct the board
 */
void destructBoard() 
{
	if (board == 0)
		return;
	for (int i = 0; i < HASH_BUCKETS_NUM; i++)
		destructList(board->hashBuckets[i]);
	free(board->hashBuckets);
	free(board);
}

/** function that must be called before the start of every game */
void start_game() {
	board = constructBoard();
}

/** 
 * function that checks the initialization status of the board
 * @return 1 if the board was initialized, 0 otherwise
 */
int isInitialized()
{
	if (board == 0)
		return 0;
	if (!board->isFirstPlayerInitialized || !board->isSecondPlayerInitialized)
		return 0;
	return 1;
}

/**
 * function that checks the status of the game
 * @return 1 if the game is over, 0 otherwise 
 */
int isGameOver()
{
	if (!isInitialized())
		return 0;
	if (board->currentMove > board->maxMoves)
		return 1;
	if (board->isFirstKingDead || board->isSecondKingDead)
		return 1;
	return 0;
}

/**
 * @param x x coordinate we need to check
 * @param y y coordinate we need to check
 * @return 1 if the coordinates are within the boundaries of the board, 0 otherwise
 */
int checkCoordinates(int x, int y)
{
	if (board == 0)
		return 0;
	if (board->size < max(x, y) || min(x, y) <= 0)
		return 0;
	return 1;
}

/**
 * function that checks the validity of a move
 * @param x1 x coordinate of the piece to be moved
 * @param y1 y coordinate of the piece to be moved
 * @param x2 target x coordinate of the piece
 * @param y2 target y coordinate of the piece
 * @return 1 if the move is valid (max(dx, dy) <= 1), 0 otherwise
 */
int checkMove(int x1, int y1, int x2, int y2)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (max(dx, dy) > 1)
		return 0;
	if (!checkCoordinates(x1, y1))
		return 0;
	if (!checkCoordinates(x2, y2))
		return 0;
	return 1;
}

/**
 * function that computes the result of a fight between two pieces
 * @param type1 the type of the first player
 * @param type2 the type of the second player
 * @return 1 if the first piece won, -1 if the second player won, 0 in case of draw
 */
int fight(PieceType type1, PieceType type2)
{
	return fightResults[type1][type2];
}

/**
 * function that matches a piece to a corresponding character representation
 * @param piece piece that must be transformed to a character
 * @return corresponding character, or '.' if the given piece was nullptr 
 */
char pieceToChar(Piece *piece)
{
	if (piece == 0)
		return '.';
	char conv[] = "rkc";
	char ret = conv[piece->type];
	if (piece->player == 1)
		ret = toupper(ret);
	return ret;
}
/** function that computes hash value of the coordinates 
 * @param x x coordinate
 * @param y y coordinate
 * @return hash value of the coordinates
 */
int computeHash(int x, int y)
{
	int ret = 0;
	ret += x;
	ret += y * HASH_QUOT;
	ret %= HASH_BUCKETS_NUM;
	if (ret < 0)
		ret *= -1;
	return ret;
}

/**
 * @param list the list of pieces where the new element must be inserted
 * @param e the piece that must be inserted
 */
PiecesList *addElementToList(PiecesList *list, Piece *e)
{
	PiecesList *ret = malloc(sizeof(PiecesList));
	ret->current = e;
	ret->next = list;
	return ret;
}

/**
 * a function that removes a piece with given coordinates from the given list
 * @param list the list from which we must destruct a piece
 * @param x x coordinate of the piece to be destructd
 * @param y y coordinate of the piece to be destructd
 * @return a new list without the removed piece
 */
PiecesList *removeFromList(PiecesList *list, int x, int y)
{
	if (list == 0)
		return 0;
	if (list->current->x == x && list->current->y == y)
	{
		destructPiece(list->current);
		PiecesList *ret = list->next;
		free(list);
		return ret;
	}
	list->next = removeFromList(list->next, x, y);
	return list;
}

/** function that searches the hash table for a piece with given coordinates
 * @param x x coordinate of the searched piece
 * @param y y coordinate of the searched piece
 * @return a piece with needed coordinates, or 0 if no piece was found
 */
Piece *getPiece(int x, int y) 
{
	if (board == 0)
		return 0;
	int bucket = computeHash(x, y);
	PiecesList *p = board->hashBuckets[bucket];
	while (p != 0)
	{
		if (p->current->x == x && p->current->y == y)
			return p->current;
		p = p->next;
	}	
	return 0;
}

/**
 * function that adds the given piece tot he board
 * @param piece the piece we want to add to our board
 * @return 1 if no error was ecnountered, 0 otherwise
 * note: when moving a piece, one should always first remove it, and then add
 * simply changing the coordinates would result in hash errors, and adding first
 * and removing later would result in status errors
 */
int addPiece(Piece *piece)
{
	if (piece == 0)
		return 0;
	if (!checkCoordinates(piece->x, piece->y))
		return 0;
	if (piece->type == king)
	{
		if (piece->player == 1)
			board->isFirstKingDead = 0;
		else if (piece->player == 2)
			board->isSecondKingDead = 0;
	}
	int bucket = computeHash(piece->x, piece->y);
	board->hashBuckets[bucket] = addElementToList(board->hashBuckets[bucket], piece);	
	return 1;
}

/**
 * a function that removes a piece with given coordinates from the board
 * @param x x coordinate of the piece to be removed
 * @param y y coordinate of the piece to be removed
 * @return 1
 **/
int removePiece(int x, int y)
{
	int bucket = computeHash(x, y);
	board->hashBuckets[bucket] = removeFromList(board->hashBuckets[bucket], x, y);
	return 1;
}

/**
 * function that places a setup line
 * @param x x coordinate of the start of setup line
 * @param y y coordinate of the start of setup line
 * @param player player the setup line belongs to
 */
int setupLine(int x, int y, int player)
{
	PieceType types[] = {king, peasant, knight, knight};
	for (int i = 0; i < 4; i++)
	{
		if (!checkCoordinates(x + i, y))
			return 0;
		if (!addPiece(constructPiece(x + i, y, player, types[i], 0)))
			return 0;
	}

	return 1; 
}

/**
 * function that checks the position of the setup line
 * @param x x coordinate of the start of setup line
 * @param y y coordinate of the start of setup line
 * @param player player the setup line must belong to
 */
int checkSetupLine(int x, int y, int player)
{
	PieceType types[] = {king, peasant, knight, knight};
	for (int i = 0; i < 4; i++)
	{
		if (!checkCoordinates(x + i, y))
			return 0;
		Piece *cur = getPiece(x + i, y);
		if (cur->type != types[i] || cur->player != player)
			return 0;
	}

	return 1; 
}


/**
 * function that prints a square part top left corner min(10, board->size) 
 */
void print_topleft() 
{
	for (int i = 1; i <= min(10, board->size); i++)
	{
		for (int j = 1; j <= min(10, board->size); j++)
			printf("%c", pieceToChar(getPiece(j, i)));
		printf("\n");
	}
	printf("\n");
}

/**
 * @param n the size of the constructed board
 * @param k maximum number of moves allowed 
 * @param p player that is being initialized
 * @param x1 x coordinate of first player's king
 * @param y1 y coordinate of first player's king
 * @param x2 x coordinate of second player's king
 * @param y2 y coordinate of second player's king
 */
int init(int n, int k, int p, int x1, int y1, int x2, int y2) 
{ 
	if (x1 == x2 && y1 == y2)
		return 0;
	if (max(abs(x1 - x2), abs(y1 - y2)) < 8)
		return 0;
	if (n < 0)
		return 0;
	if (k <= 0)
		return 0;
	if (p <= 0 || p > 2)
		return 0;
	if (p == 1)
	{
		if (board->isFirstPlayerInitialized)
			return 0;
		else
			board->isFirstPlayerInitialized = 1;
	}
	else if (p == 2)
	{
		if (board->isSecondPlayerInitialized)
			return 0;
		else
			board->isSecondPlayerInitialized = 1;
	}

	if (board->size == -1)
	{
		board->size = n;
		board->maxMoves = k;
		if (!setupLine(x1, y1, 1))
			return 0;

		if (!setupLine(x2, y2, 2))
			return 0;
	}
	else 
	{
		if (board->size != n)
			return 0;

		if (board->maxMoves != k)
			return 0;

		if (!checkSetupLine(x1, y1, 1))
			return 0;

		if (!checkSetupLine(x2, y2, 2))
			return 0;
	}
	return 1;
}

/**
 * a function that moves a piece from one place to another
 * @param x1 x coordinate of the piece to be moved
 * @param y1 y coordinate of the piece to be moved
 * @param x2 target x coordinate of the piece
 * @param y2 target y coordinate of the piece
 * @return 1 if no error was encountered, 0 otherwise
 * */
int move(int x1, int y1, int x2, int y2) 
{
	if (isGameOver())
		return 0;
	if (!checkMove(x1, y1, x2, y2))
		return 0;
	Piece *piece = getPiece(x1, y1);
	if (piece == 0) 
		return 0;
	//printf("%d %d\n", piece->player, board->currentPlayer);
	if (piece->player != board->currentPlayer)
	       return 0;
	if (piece->lastMove > board->currentMove - 1)
		return 0;

	Piece *target = getPiece(x2, y2);
	if (target == 0)
	{
		Piece *temp = constructPiece(x2, y2, piece->player, piece->type, board->currentMove);
		removePiece(x1, y1);
		addPiece(temp);
	}
	else
	{
		if (target->player == piece->player) 
			return 0;
		int f = fight(piece->type, target->type);
		if (f == 0)
		{
			removePiece(x1, y1);
			removePiece(x2, y2);
		}
		else if (f == 1)
		{
			removePiece(x2, y2);
			Piece *temp = constructPiece(x2, y2, piece->player, piece->type, board->currentMove);
			removePiece(x1, y1);
			addPiece(temp); 
		}
		else if (f == -1)
		{
			removePiece(x1, y1);	
		}
	}
	return 1;
}

/** a function that produces a piece of the given type 
 * @param x1 x coordinate of the producing peasant
 * @param y1 y coordinate of the producing peasant
 * @param x2 x coordinate of the piece to be produced
 * @param y2 y coordinate of the piece to be produced
 * @param type type of the piece to be produced
 * @return 1 if no error was encountered, 0 otherwise
 */
int produce(int x1, int y1, int x2, int y2, PieceType type)
{
	if (isGameOver())
		return 0;
	if (!checkMove(x1, y1, x2, y2))
		return 0;
	Piece *piece = getPiece(x1, y1);
	//printf("%d %d\n", piece->player, board->currentPlayer);
	if (piece == 0 || piece->type != peasant || piece->player != board->currentPlayer || piece->lastMove > board->currentMove - 3)
		return 0;
	Piece *target = getPiece(x2, y2);	
	piece->lastMove = board->currentMove;
	if (target == 0)
	{
		addPiece(constructPiece(x2, y2, board->currentPlayer, type, board->currentMove - 1));
		return 1;
	}
	else
	{
		if (target->player == board->currentPlayer)
			return 0;
		int f = fight(type, target->type);
		if (f == 0)
		{
			removePiece(x2, y2);
		}
		else if (f == 1)
		{
			removePiece(x2, y2);
			addPiece(constructPiece(x2, y2, board->currentPlayer, type, board->currentMove - 1));
		}
	}
	return 1;
}

/** a function that produces a knight
 * @param x1 x coordinate of the producing peasant
 * @param y1 y coordinate of the producing peasant
 * @param x2 x coordinate of the piece to be produced
 * @param y2 y coordinate of the piece to be produced
 * @return 1 if no error was encountered, 0 otherwise
 */
int produce_knight(int x1, int y1, int x2, int y2) 
{
	return produce(x1, y1, x2, y2, knight);
}

/** a function that produces a peasant
 * @param x1 x coordinate of the producing peasant
 * @param y1 y coordinate of the producing peasant
 * @param x2 x coordinate of the piece to be produced
 * @param y2 y coordinate of the piece to be produced
 * @return 1 if no error was encountered, 0 otherwise
 */
int produce_peasant(int x1, int y1, int x2, int y2) 
{
	return produce(x1, y1, x2, y2, peasant);
}

/** a function that ends the current turn
 * @return 1 if no error was encountered, 0 otherwise
 */
int end_turn() 
{
	if (!isInitialized())
		return 0;
	if (board->currentPlayer == 1)
	{
		board->currentPlayer = 2;
	}
	else
	{
		board->currentPlayer = 1;
		board->currentMove++;
	}
	return 1;
}

/** a function that destructs the board
 * @return 1 if no error was encountered, 0 otherwise
 */
int end_game() 
{
	int ret;
	if (board->isSecondKingDead && !board->isFirstKingDead)
		ret = 1;
	else if (!board->isSecondKingDead && board->isFirstKingDead)
		ret = 2;
	else
		ret = 0;
	destructBoard();
	return ret;
}
