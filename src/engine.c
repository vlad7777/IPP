#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define KNIGHT_CODE 0
#define KING_CODE 1
#define PEASANT_CODE 2

#define HASH_BUCKETS_NUM 1003
#define HASH_QUOT 23LL

int max(int a, int b)
{
	if (a < b)
		return b;
	else
		return a;
}

int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

typedef enum def_PieceType 
{
	knight = KNIGHT_CODE,
	king = KING_CODE,
	peasant = PEASANT_CODE 
} PieceType;

int fightResults[3][3] = {
	{0, 1, 1},
	{-1, 0, 1},
	{-1, -1, 0} };

typedef struct def_Piece 
{
	PieceType type;	
	int team;
	int x, y;
	int lastMove;
} Piece;

typedef struct def_PiecesList {
	Piece *current;
	struct def_PiecesList *next;
} PiecesList;

typedef struct def_Board {
	int size;
	int isFinished;
	int maxMoves;
	PiecesList **hashBuckets;
	int currentMove;
	int currentPlayer;
	int isFirstPlayerInitialized;
	int isSecondPlayerInitialized;
	int isFirstKingDead;
	int isSecondKingDead;

} Board;

Board *board = 0;

Board* constructBoard()
{
	Board *ret = malloc(sizeof(Board));	
	ret->size = -1;
	ret->hashBuckets = calloc(HASH_BUCKETS_NUM, sizeof(PiecesList*));
	ret->currentMove = 1;
	ret->currentPlayer = 1;
	ret->isFinished = 0;
	ret->isSecondKingDead = 0;
	ret->isFirstKingDead = 0;
	ret->isSecondPlayerInitialized = 0;
	ret->isFirstPlayerInitialized = 0;
	return ret;
}

void start_game() {
	board = constructBoard();	
}

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

Piece *getPiece(Board *board, int x, int y) 
{
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

char pieceToChar(Piece *piece)
{
	if (piece == 0)
		return '.';
	char conv[] = "rkc";
	char ret = conv[piece->type];
	if (piece->team == 1)
		ret = toupper(ret);
	return ret;
}

void print_topleft() 
{
	for (int i = 1; i <= min(10, board->size); i++)
	{
		for (int j = 1; j <= min(10, board->size); j++)
			printf("%c", pieceToChar(getPiece(board, j, i)));
		printf("\n");
	}
	printf("\n");
}

int isInitialized()
{
	if (board == 0)
		return 0;
	if (!board->isFirstPlayerInitialized || !board->isSecondPlayerInitialized)
		return 0;
	return 1;
}

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

int checkCoordinates(Board *board, int x, int y)
{
	if (board->size < max(x, y) || min(x, y) <= 0)
		return 0;
	return 1;
}

PiecesList *addElementToList(PiecesList *list, Piece *e)
{
	PiecesList *ret = malloc(sizeof(PiecesList));
	ret->current = e;
	ret->next = list;
	return ret;
}

int addPiece(Board *board, Piece *piece)
{
	if (!checkCoordinates(board, piece->x, piece->y))
		return 0;
	if (piece->type == king)
	{
		if (piece->team == 1)
			board->isFirstKingDead = 0;
		else if (piece->team == 2)
			board->isSecondKingDead = 0;
	}
	int bucket = computeHash(piece->x, piece->y);
	board->hashBuckets[bucket] = addElementToList(board->hashBuckets[bucket], piece);	
	return 1;
}

Piece *constructPiece(int x, int y, int player, PieceType type, int lastMove)
{
	Piece *ret = malloc(sizeof(Piece));
	ret->x = x;
	ret->y = y;
	ret->team = player;
	ret->type = type;
	ret->lastMove = lastMove; 
	return ret;
}

void destructPiece(Piece *piece)
{
	if (piece->type == king)
	{
		if (piece->team == 1)
			board->isFirstKingDead = 1;
		else 
			board->isSecondKingDead = 1;
	}	
	free(piece);
}

int setupLine(Board *board, int x, int y, int player)
{
	PieceType types[] = {king, peasant, knight, knight};
	for (int i = 0; i < 4; i++)
	{
		if (!checkCoordinates(board, x + i, y))
			return 0;
		if (!addPiece(board, constructPiece(x + i, y, player, types[i], 0)))
			return 0;
	}

	return 1; 
}

int checkSetupLine(Board *board, int x, int y, int player)
{
	PieceType types[] = {king, peasant, knight, knight};
	for (int i = 0; i < 4; i++)
	{
		if (!checkCoordinates(board, x + i, y))
			return 0;
		Piece *cur = getPiece(board, x + i, y);
		if (cur->type != types[i] || cur->team != player)
			return 0;
	}

	return 1; 
}

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
		if (!setupLine(board, x1, y1, 1))
			return 0;

		if (!setupLine(board, x2, y2, 2))
			return 0;
	}
	else 
	{
		if (board->size != n)
			return 0;

		if (board->maxMoves != k)
			return 0;

		if (!checkSetupLine(board, x1, y1, 1))
			return 0;

		if (!checkSetupLine(board, x2, y2, 2))
			return 0;
	}
	return 1;
}

int checkMove(int x1, int y1, int x2, int y2)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (max(dx, dy) > 1)
		return 0;
	if (!checkCoordinates(board, x1, y1))
		return 0;
	if (!checkCoordinates(board, x2, y2))
		return 0;
	return 1;
}

int fight(PieceType type1, PieceType type2)
{
	return fightResults[type1][type2];
}

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

void deleteList(PiecesList *list)
{
	if (list == 0)
		return;
	deleteList(list->next);
	free(list->current);
	free(list);
}

void deleteBoard() 
{
	if (board == 0)
		return;
	for (int i = 0; i < HASH_BUCKETS_NUM; i++)
		deleteList(board->hashBuckets[i]);
	free(board->hashBuckets);
	free(board);
}

int removePiece(Board *board, int x, int y)
{
	int bucket = computeHash(x, y);
	board->hashBuckets[bucket] = removeFromList(board->hashBuckets[bucket], x, y);
	return 1;
}

int move(int x1, int y1, int x2, int y2) 
{
	if (isGameOver())
		return 0;
	if (!checkMove(x1, y1, x2, y2))
		return 0;
	Piece *piece = getPiece(board, x1, y1);
	if (piece == 0) 
		return 0;
	//printf("%d %d\n", piece->team, board->currentPlayer);
	if (piece->team != board->currentPlayer)
	       return 0;
	if (piece->lastMove > board->currentMove - 1)
		return 0;

	Piece *target = getPiece(board, x2, y2);
	if (target == 0)
	{
		Piece *temp = constructPiece(x2, y2, piece->team, piece->type, board->currentMove);
		removePiece(board, x1, y1);
		addPiece(board, temp);
	}
	else
	{
		if (target->team == piece->team) 
			return 0;
		int f = fight(piece->type, target->type);
		if (f == 0)
		{
			removePiece(board, x1, y1);
			removePiece(board, x2, y2);
		}
		else if (f == 1)
		{
			removePiece(board, x2, y2);
			Piece *temp = constructPiece(x2, y2, piece->team, piece->type, board->currentMove);
			removePiece(board, x1, y1);
			addPiece(board, temp); 
		}
		else if (f == -1)
		{
			removePiece(board, x1, y1);	
		}
	}
	return 1;
}

int produce(int x1, int y1, int x2, int y2, PieceType type)
{
	if (isGameOver())
		return 0;
	if (!checkMove(x1, y1, x2, y2))
		return 0;
	Piece *piece = getPiece(board, x1, y1);
	//printf("%d %d\n", piece->team, board->currentPlayer);
	if (piece == 0 || piece->type != peasant || piece->team != board->currentPlayer || piece->lastMove > board->currentMove - 3)
		return 0;
	Piece *target = getPiece(board, x2, y2);	
	piece->lastMove = board->currentMove;
	if (target == 0)
	{
		addPiece(board, constructPiece(x2, y2, board->currentPlayer, type, board->currentMove - 1));
		return 1;
	}
	else
	{
		if (target->team == board->currentPlayer)
			return 0;
		int f = fight(type, target->type);
		if (f == 0)
		{
			removePiece(board, x2, y2);
		}
		else if (f == 1)
		{
			removePiece(board, x2, y2);
			addPiece(board, constructPiece(x2, y2, board->currentPlayer, type, board->currentMove - 1));
		}
	}
	return 1;
}

int produce_knight(int x1, int y1, int x2, int y2) 
{
	return produce(x1, y1, x2, y2, knight);
}

int produce_peasant(int x1, int y1, int x2, int y2) 
{
	return produce(x1, y1, x2, y2, peasant);
}

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

int end_game() 
{
	int ret;
	if (board->isSecondKingDead && !board->isFirstKingDead)
		ret = 1;
	else if (!board->isSecondKingDead && board->isFirstKingDead)
		ret = 2;
	else
		ret = 0;
	deleteBoard();
	return ret;
}
