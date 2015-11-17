#ifndef CHESS_
#define CHESS_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL_image.h>

#include <math.h>

#include <time.h>





/* structs */
struct checker{
	char x;
	int  y;
};
typedef struct checker checker;

struct legalMovesRepository{
	int size;
	int pos;
	char **legalMoves;
};
typedef struct legalMovesRepository legalMovesRepository;

struct treeNode
{
	struct treeNode **branch;
	int boardValue;
	char *move;
	int childAmount;

};
typedef struct treeNode treeNode;


/* enums */
typedef enum { BLACK, WHITE } color;
typedef enum { PAWN, KING } rank;


/* macros */

#define OPPOSITE_COLOR(clr) ((clr == WHITE)? BLACK: WHITE)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define INF 2147483647

#define WHITE_P 'm'
#define WHITE_B 'b'
#define WHITE_R 'r'
#define WHITE_N 'n'
#define WHITE_Q 'q'
#define WHITE_K 'k'
#define WHITE_TOOL(tool) ((tool==WHITE_P || tool==WHITE_B || tool==WHITE_R || tool==WHITE_N || tool==WHITE_Q ||  tool==WHITE_K) ? 1:0)

#define BLACK_P 'M'
#define BLACK_B 'B'
#define BLACK_R 'R'
#define BLACK_N 'N'
#define BLACK_Q 'Q'
#define BLACK_K 'K'
#define BLACK_TOOL(tool) ((tool==BLACK_P || tool==BLACK_B || tool==BLACK_R || tool==BLACK_N || tool==BLACK_Q ||  tool==BLACK_K) ? 1:0)

#define EMPTY ' '

#define SCORE_P 1
#define SCORE_N 3
#define SCORE_B 3
#define SCORE_R 5
#define SCORE_Q 9
#define SCORE_K 400

#define BOARD_SIZE 8
#define STRING_SIZE 50 
#define NUM_OF_REPS 20
#define BOARD_SIZE 8
#define MAX_NODES 1000000


#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. Value should be between 1 to 4\n"
#define WRONG_GAME_MODE "Wrong game mode\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_DICS "The specified position does not contain your piece\n"
#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
#define ILLEGAL_CASTLE "Illegal casteling move\n"
#define NO_ROOK "Wrong position for a rook\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"
#define WRONG_FILE "Wrong file name\n"

#define perror_message(func_name) (perror("Error: standard function %s has failed", func_name))
#define print_message(message) (printf("%s", message));


extern treeNode *gameTree;
extern int minimaxDepth;
extern int game_mode;
extern color user_color;
extern int initDepth;



/* methods declaration */

/* board methods */
void print_line();
void print_board_(char board[BOARD_SIZE][BOARD_SIZE]);
void init_board_(char board[BOARD_SIZE][BOARD_SIZE]);
void clear_board(char board[BOARD_SIZE][BOARD_SIZE]);
void copyBoard(char oldBoard[BOARD_SIZE][BOARD_SIZE], char newBoard[BOARD_SIZE][BOARD_SIZE]);
int boardScore(char board[BOARD_SIZE][BOARD_SIZE], color curColor);
int bestBoardScore(char board[BOARD_SIZE][BOARD_SIZE], color curColor);

/* prime methods */
color settings_state(char board[BOARD_SIZE][BOARD_SIZE]);
void game_state(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color);
void user_turn(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color);
void computer_turn(char board[BOARD_SIZE][BOARD_SIZE], color current_player_olor);
void simulateTurn(char oldBoard[BOARD_SIZE][BOARD_SIZE], char* move, char newBoard[BOARD_SIZE][BOARD_SIZE]);


/* moves methods */
void getAllLegalMoves(char board[BOARD_SIZE][BOARD_SIZE], color player_color, legalMovesRepository* rep, int checkThreat, int depthFlag);
void getLegalMoves(char board[BOARD_SIZE][BOARD_SIZE], color player_color, checker srcChkr, legalMovesRepository* rep, int checkThreat);
int underAttack(int i, int j, char board[BOARD_SIZE][BOARD_SIZE], color attacker);
int isKingSafe(char board[BOARD_SIZE][BOARD_SIZE], color c);
int isItLegal( char board[BOARD_SIZE][BOARD_SIZE], char *move, color c);



/* minor helping methods */
int pieceToNum(char c);
checker stringToChecker(char* str);
void substringFromString(char* str, int i, int j, char *cpy);
void convertMoveToString(char* str, checker srcChkr, checker dstChkr);
void convertCheckerToString(char* str, checker chkr);


/* repository methods */
void addPromotionMoveToRep (char *str, legalMovesRepository* rep );
void addMoveToRepository(legalMovesRepository* rep, char* str);
int isItInRepository(legalMovesRepository* rep, char* str);
void freeRepository(legalMovesRepository* repository);
void freeAllRepositories();
void sortRepository(char board[BOARD_SIZE][BOARD_SIZE], legalMovesRepository* rep, color current_player_color, int ascending);
void initArrayOfReps();
int addRepToArrayOfReps(legalMovesRepository* rep);
void removeRepFromArrayOfReps(legalMovesRepository* rep);


/* wrappers*/
void scanfWrapper(int num);
void* allocWrapper(void* ptr, char* allocName);


/* game tree methods */
int getBestScoreBybuildingGameTree(char board[BOARD_SIZE][BOARD_SIZE], int depth, color PlayerA, color currentColor,
char* curMove, treeNode* father, int whichSon, int alpha, int beta, int maximizingPlayer);
void freeTree(treeNode *tree);
char* chooseMove(int bestValue, treeNode *tree, int randomly);

/* file methods */
int saveGame(char board[BOARD_SIZE][BOARD_SIZE], char* filename, color current_player_color);
int loadGame(char board[BOARD_SIZE][BOARD_SIZE], char* filename, color* current_player_color);

/* GUI methods */
SDL_Surface *load_image( char *filename, int transparent ) ;
void apply_surface (int x, int  y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect* clip);
void initWindow ();

int guiSplashScreen (char board[BOARD_SIZE][BOARD_SIZE], color* current_player_color_pointer);
void guiLoadScreen (char board[BOARD_SIZE][BOARD_SIZE], color* current_player_color_pointer);
void loadState(char board[BOARD_SIZE][BOARD_SIZE], int state, color* current_player_color_pointer);
void guiSettingsState (char board[BOARD_SIZE][BOARD_SIZE]);
void refresh();
void chooseGameType (char board[BOARD_SIZE][BOARD_SIZE]);
void guiWinMessage(color c, char board[BOARD_SIZE][BOARD_SIZE], color current_color);
char * guiGetMove(char board[BOARD_SIZE][BOARD_SIZE],color current_player_color);
void moveError ();
char * choosePromotion();
void guiCheckMessage (char board[BOARD_SIZE][BOARD_SIZE]);
void saveState (char board[BOARD_SIZE][BOARD_SIZE], color c);
void guiUserTurn (char board[BOARD_SIZE][BOARD_SIZE], color current_player_color);
int guiGetSaveSlot();
/* gui set functions */
void guiSetMainStage (char board[BOARD_SIZE][BOARD_SIZE]);
void setBoard (char board[BOARD_SIZE][BOARD_SIZE], int piecesOnly );
void setBelowBoard(color current_player_color);

void drawSaveScreen ();
void drawLoadScreen ();


#endif
CHESS_
































