#include "Chess.h"


/* global variables */

treeNode *gameTree = NULL;
treeNode *prevGameTree = NULL;
int current_num_of_nodes;
int minimaxDepth;
int game_mode;
color user_color;

int restartFlag = 0;


int GUI;
int done = 0;
int initDepth = 0;
int nodeNum = 0;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int SCREEN_BPP = 0;
int SAVE_STATES = 5;
int LOAD = 0;

SDL_Surface *screen = NULL; /* main screen */
SDL_Rect clip[14]; /* sprite map handling */
SDL_Surface *tools = NULL; /* game tools sprite sheet */
SDL_Surface *guiBoard = NULL; /* game board */


int main(int argc, char ** argv)
{
	/* creating the board */
	char board[BOARD_SIZE][BOARD_SIZE];
	color current_player_color = WHITE; /*by default player starts */
	color *current_player_color_pointer = &current_player_color;

	/* initializing the game with default values */
	current_num_of_nodes = 0;


	minimaxDepth = 1;
	initDepth = minimaxDepth;
	game_mode = 1;
	user_color = WHITE;
	init_board_(board);
	initArrayOfReps();
	if (SAVE_STATES > 10)
		SAVE_STATES = 10;

	if (argc == 0) /* restart from gui */
		goto GUI_RESTART;

	/* GUI flag */
	if (argc == 1)
		GUI = 0;
	else if (!strcmp(argv[1], "console") && argc == 2)
		GUI = 0;
	else if (!strcmp(argv[1], "gui") && argc == 2)
		GUI = 1;

GUI_RESTART:
	if (GUI == 1){

		/* start of UI tree */
		guiSplashScreen(board, current_player_color_pointer);


	}
	else if (GUI == 0){ /* cmd mode */
		/* settings and game states */
		current_player_color = settings_state(board);
		game_state(board, current_player_color);
	}


	return 0;
}

/* *******************************************************/
/* ***************  prime methods   **********************/
/* *******************************************************/


/*
the function gets a board and runs the settings state as described in the PDF of this exercise.
It returns the color of the player who should play next (For new game its WHITE by default).
*/
color settings_state(char board[BOARD_SIZE][BOARD_SIZE]){
	char input[STRING_SIZE];
	char arg[STRING_SIZE], a[STRING_SIZE];
	int x, flag = 0;
	color current_player_color = WHITE;
	color* current_player_color_pointer = &current_player_color;



	if (GUI != 1)
		print_board_(board); /* cmd mode, print board out */


	input[0] = '\0';
	printf("Enter game setting:\n");

	while (1){
		/* clearing input buffer */
		if (flag)
		while (getchar() != '\n');

		flag = 1;
		scanfWrapper(scanf("%s", input));

		if (strcmp(input, "game_mode") == 0){
			scanfWrapper(scanf("%d", &x));
			if (x != 1 && x != 2){
				print_message(WRONG_GAME_MODE);
				continue;
			}
			game_mode = x;
			if (x == 1)
				printf("Running game in 2 players mode\n");
			else
				printf("Running game in player vs. AI mode\n");

		}
		else if (strcmp(input, "difficulty") == 0 && game_mode == 2){
			scanfWrapper(scanf("%s", a));

			if (strcmp(a, "best") == 0){
				minimaxDepth = -1;
				continue;
			}

			if (strcmp(a, "depth") == 0){
				scanfWrapper(scanf("%d", &x));
				if (x<1 || x>4){
					print_message(WRONG_MINIMAX_DEPTH);
					continue;
				}
				minimaxDepth = x;
				initDepth = minimaxDepth;
				continue;
			}

			print_message(ILLEGAL_COMMAND);

		}
		else if (strcmp(input, "user_color") == 0 && game_mode == 2){
			scanfWrapper(scanf("%s", arg));
			if (strcmp(arg, "black") == 0)
				user_color = BLACK;
			else
				user_color = WHITE;
		}
		else if (strcmp(input, "load") == 0){
			/* getting the filename  */
			scanfWrapper(scanf("%s", arg));

			if (loadGame(board, arg, current_player_color_pointer) == 0){
				print_message(WRONG_FILE);
				continue;
			}
			LOAD = 1; /* game starts from load */
			print_board_(board);

		}


		else if (strcmp(input, "print") == 0){
			print_board_(board);
		}
		else if (strcmp(input, "quit") == 0){
			freeAllRepositories();
			freeTree(gameTree);
			exit(0);
		}
		else if (strcmp(input, "start") == 0){
			break;
		}
		else{
			print_message(ILLEGAL_COMMAND);
		}


	}

	/* settings state is over */
	return current_player_color;
}


/*
the function gets a board and the current player color
and runs the game state as described in the PDF of this exercise.
*/
void game_state(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){

	legalMovesRepository re;
	legalMovesRepository* rep = &re;


	LOAD = 0;


	/* initialize repository */
	rep->legalMoves = NULL;
	rep->pos = 0; rep->size = 0;
	addRepToArrayOfReps(rep);



	getAllLegalMoves(board, OPPOSITE_COLOR(current_player_color), rep, 1, 0);
	/* checking for a loaded win game */
	if (rep->size == 0){
		if (current_player_color == WHITE){
			if (GUI != 1)
				printf("Mate! White player wins the game\n");
			else{
				guiWinMessage(WHITE, board, current_player_color); /* black won */
			}
		}
		else{
			if (GUI != 1)
				printf("Mate! Black player wins the game\n");
			else{
				guiWinMessage(BLACK, board, current_player_color); /* white won  */
			}
		}


		freeAllRepositories();
		freeTree(gameTree);
		exit(0);
	}
	/* finished with this repository*/
	freeRepository(rep);

	/* initializing a new repository for inside the while loop */
	rep->legalMoves = NULL;
	rep->pos = 0; rep->size = 0;
	addRepToArrayOfReps(rep);

	while (1){


		if (GUI == 1){

			setBelowBoard(current_player_color);
		}

		if (current_player_color == user_color || game_mode == 1){

			user_turn(board, current_player_color);
		}
		else{

			computer_turn(board, current_player_color);
		}

		if (GUI != 1)
			print_board_(board); /* cmd mode, print board out */
		else{

			setBoard(board, 0); /* gui mode, show board on screen */


		}

		

		/* creating a repository of all legal available moves of the OPPONENT!!! */
		getAllLegalMoves(board, OPPOSITE_COLOR(current_player_color), rep, 1, 0);

		/* if the user has no moves to play, he lost */
		if (rep->size == 0){
			if (current_player_color == BLACK){
				if (GUI != 1)
					printf("Mate! Black player wins the game\n");
				else{
					guiWinMessage(BLACK, board, current_player_color); /* black won */
				}
			}
			else{
				if (GUI != 1)
					printf("Mate! White player wins the game\n");
				else{
					guiWinMessage(WHITE, board, current_player_color); /* white won  */
				}
			}


			freeAllRepositories();
			freeTree(gameTree);
			exit(0);
		}
		else{
			/* checking if it a check*/
			if (!isKingSafe(board, OPPOSITE_COLOR(current_player_color))){
				if (GUI != 1)
					printf("Check!\n");
				else
					guiCheckMessage(board);
			}
		}

		freeRepository(rep);
		/* initializing a new repository for inside the while loop */
		rep->legalMoves = NULL;
		rep->pos = 0; rep->size = 0;
		addRepToArrayOfReps(rep);



		/* changing the current player color to the color of the next player */
		current_player_color = OPPOSITE_COLOR(current_player_color);



	}

	freeRepository(rep);
}


/* this function simulates a user turn */
void user_turn(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){
	char cmd[STRING_SIZE], str[STRING_SIZE], str2[STRING_SIZE], theMove[STRING_SIZE], promoteTo[STRING_SIZE];

	char ch;
	checker srcChkr;
	checker destChkr;
	legalMovesRepository re;
	legalMovesRepository* rep = &re;
	int i, clearBufferFlag = 0;

	if (GUI == 1){
		/* GUI mode */
		/* show game window and get move */
		guiUserTurn(board, current_player_color);

	}
	else{

		/* initialize repository */
		rep->legalMoves = NULL;
		rep->pos = 0; rep->size = 0;
		addRepToArrayOfReps(rep);

		/* creating a repository of all legal available moves */
		getAllLegalMoves(board, current_player_color, rep, 1, 0);


		while (1){


			if (current_player_color == BLACK)
				printf("Black player - enter your move:\n");
			else
				printf("White player - enter your move:\n");
			/* clearing input buffer */
			if (clearBufferFlag)
			while (getchar() != '\n');

			clearBufferFlag = 1;

			/* getting new command */
			scanfWrapper(scanf("%s", cmd));

			if (strcmp(cmd, "move") == 0){
				/* getting the source checker */
				scanfWrapper(scanf("%s", str));
				strcpy(theMove, str);
				srcChkr = stringToChecker(str);
				if (srcChkr.y == 0){
					print_message(WRONG_POSITION);
					continue;
				}

				/* getting the 'to' */
				scanfWrapper(scanf("%s", str));
				strcat(theMove, " to ");
				if (strcmp(str, "to") != 0)
				{
					print_message(ILLEGAL_COMMAND);
					continue;
				}

				/* getting the destination checker */
				scanfWrapper(scanf("%s", str));
				strcat(theMove, str);



				/* creating an array of destination checkers */
				destChkr = stringToChecker(str);

				/* making sure the checker is legal */
				if (destChkr.y == 0){
					print_message(WRONG_POSITION);
					continue;
				}


				/* checking if the user has a tool on the source checker */
				if (!(((current_player_color == WHITE) &&
					WHITE_TOOL(board[srcChkr.x - 97][srcChkr.y - 1])) ||
					((current_player_color == BLACK) &&
					BLACK_TOOL(board[srcChkr.x - 97][srcChkr.y - 1]))))
				{
					print_message(NO_DICS);
					continue;
				}

				/* getting the extra string if a pawn has to be promoted */
				if (((board[srcChkr.x - 97][srcChkr.y - 1] == WHITE_P && destChkr.y == BOARD_SIZE) ||
					(board[srcChkr.x - 97][srcChkr.y - 1] == BLACK_P && destChkr.y == 1)) && isItInRepository(rep, theMove))
				{
					i = 0;
					while ((ch = getchar()) != '\n')
						promoteTo[i++] = ch;
					promoteTo[i] = '\0';
					clearBufferFlag = 0;

					if (strcmp(promoteTo, "") == 0){
						strcpy(promoteTo, " queen");	 /* queen by default*/
					}
					else if (strcmp(promoteTo, " knight") != 0 && strcmp(promoteTo, " bishop") != 0 &&
						strcmp(promoteTo, " rook") != 0 && strcmp(promoteTo, " queen") != 0) {
						print_message(ILLEGAL_COMMAND);
						continue;
					}

					strcat(theMove, promoteTo);

				}

				/* checking if it a legal move by searching in the repository */
				if (isItInRepository(rep, theMove) == 0){
					print_message(ILLEGAL_MOVE);
					continue;

				}

				/* the move is legal - update the board */
				simulateTurn(board, theMove, board);


				break;

			}
			else if (strcmp(cmd, "get_moves") == 0){
				scanfWrapper(scanf("%s", str));
				srcChkr = stringToChecker(str);

				if (srcChkr.y == 0){
					print_message(WRONG_POSITION);
					continue;
				}

				/* checking if the user has a tool on the source checker */
				if (!(((current_player_color == WHITE) &&
					WHITE_TOOL(board[srcChkr.x - 97][srcChkr.y - 1])) ||
					((current_player_color == BLACK) &&
					BLACK_TOOL(board[srcChkr.x - 97][srcChkr.y - 1]))))
				{
					print_message(NO_DICS);
					continue;
				}

				for (i = 0; i < rep->size; i++){
					substringFromString(str2, 0, 4, rep->legalMoves[i]);

					if (strcmp(str, str2) == 0 && strlen(rep->legalMoves[i]) > 7)
						/* print regular move */
						printf("%s\n", rep->legalMoves[i]);

				}


			}
			else if (strcmp(cmd, "restart") == 0){

				main(1, NULL); /* restart and go to console mode */
			}
			else if (strcmp(cmd, "save") == 0){
				/* getting the filename  */
				scanfWrapper(scanf("%s", str));

				if (saveGame(board, str, current_player_color) == 0){
					print_message(WRONG_FILE);
					continue;
				}

			}
			else if (strcmp(cmd, "quit") == 0){
				freeAllRepositories();
				freeTree(gameTree);
				exit(0);
			}
			else{
				print_message(ILLEGAL_COMMAND);

			}
		}

		freeRepository(rep);
	}
}



/* this function simulates a computer turn */
void computer_turn(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){
	color PC_Color;
	char *chosenMove;
	int bestValue, prevBest = 0;
	int depth = 1;


	nodeNum = 0;

	if (minimaxDepth == -1){

		PC_Color = OPPOSITE_COLOR(user_color);

		while (nodeNum < MAX_NODES){

			initDepth = depth;
			bestValue = getBestScoreBybuildingGameTree(board, depth, PC_Color, PC_Color, NULL, NULL, 0, -1 * INF, INF, 1);
			if (nodeNum < MAX_NODES){
				prevBest = bestValue;
				depth++;

				freeTree(prevGameTree);
				prevGameTree = gameTree;
				gameTree = NULL;

				if (prevBest == INF)
					break;/* only one move option, just stop */
			}
			else{
				freeTree(gameTree);
				gameTree = NULL;
			}


		}

		gameTree = prevGameTree;
		chosenMove = chooseMove(prevBest, gameTree, 0);


		initDepth = depth;

	}
	else{

		depth = minimaxDepth;

		PC_Color = OPPOSITE_COLOR(user_color);

		bestValue = getBestScoreBybuildingGameTree(board, depth, PC_Color, PC_Color, NULL, NULL, 0, -1 * INF, INF, 1);
		chosenMove = chooseMove(bestValue, gameTree, 0);
	}

	simulateTurn(board, chosenMove, board); /* update board */

	if( GUI != 1)
		printf("move %s\n",chosenMove);

	freeTree(gameTree);
	gameTree = NULL;
	prevGameTree = NULL;

}


/* gets an old board, a move string and a new board
at the end -  the new board looks like the old board after playing the given move
*/
void simulateTurn(char oldBoard[BOARD_SIZE][BOARD_SIZE], char* move, char newBoard[BOARD_SIZE][BOARD_SIZE]){
	char srcStr[STRING_SIZE], destStr[STRING_SIZE];
	char ch;
	checker srcChkr;
	checker destChkr;


	/* source checker */
	substringFromString(srcStr, 0, 4, move);
	srcChkr = stringToChecker(srcStr);


	/* destination checker */
	substringFromString(destStr, 9, 13, move);
	destChkr = stringToChecker(destStr);

	/* the new board should be identical to the old one at the beggining */
	copyBoard(oldBoard, newBoard);

	/* if a pawn got to the far side it is promoted */
	if ((oldBoard[srcChkr.x - 97][srcChkr.y - 1] == WHITE_P && destChkr.y == BOARD_SIZE) ||
		(oldBoard[srcChkr.x - 97][srcChkr.y - 1] == BLACK_P && destChkr.y == 1))
	{
		if (strlen(move) < 15)
			ch = (oldBoard[srcChkr.x - 97][srcChkr.y - 1] == WHITE_P) ? 'q' : 'Q';
		else
			ch = move[15];

		if (ch == 'k' && oldBoard[srcChkr.x - 97][srcChkr.y - 1] == WHITE_P)
			ch = 'n';
		else if (ch == 'k' && oldBoard[srcChkr.x - 97][srcChkr.y - 1] == BLACK_P)
			ch = 'N';
		else if (ch == 'q' && oldBoard[srcChkr.x - 97][srcChkr.y - 1] == BLACK_P)
			ch = 'Q';
		else if (ch == 'r' && oldBoard[srcChkr.x - 97][srcChkr.y - 1] == BLACK_P)
			ch = 'R';
		else if (ch == 'b' && oldBoard[srcChkr.x - 97][srcChkr.y - 1] == BLACK_P)
			ch = 'B';

	}
	else
		ch = newBoard[srcChkr.x - 97][srcChkr.y - 1];

	/* simulate the move */
	newBoard[destChkr.x - 97][destChkr.y - 1] = ch;
	newBoard[srcChkr.x - 97][srcChkr.y - 1] = EMPTY;

}




/* *******************************************************/
/* **************  game tree methods   *******************/
/* *******************************************************/
/*
The method gets:
1) board             - a board game
2) depth             - the depth of the tree
3) PlayerA           - the color of the player which this tree is being created for
4) currentColor      - the color of the current player (in this level of the tree)
5) curMove		     - used to transfer the move from a father to his child (in order to store it in the child)
6) father		     - pointer to the father of the current node created
7) whichSon		     - indicates what is the serial number of this child regarding to his father
8) alpha		     - used for alpha beta pruning
9) beta			     - used for alpha beta pruning
10) maximizingPlayer - indicated whether this level we should max or min (it is a version of minimax algorithm afterall)


The methods returns the best score (of the best board) for PlayerA (using alpha beta pruning).

*/

int getBestScoreBybuildingGameTree(char board[BOARD_SIZE][BOARD_SIZE], int depth, color PlayerA, color currentColor,
	char* curMove, treeNode* father, int whichSon, int alpha, int beta, int maximizingPlayer){

	char nextBoard[BOARD_SIZE][BOARD_SIZE];
	treeNode *root = NULL; treeNode *son = NULL;
	legalMovesRepository re;
	legalMovesRepository *rep = &re;
	int i, amountOfMoves, temp;
	color nextColor;

	/* set next level color */
	nextColor = OPPOSITE_COLOR(currentColor);

	/* initialize repository */
	rep->legalMoves = NULL;
	rep->pos = 0; rep->size = 0;
	addRepToArrayOfReps(rep);

	/* get all possible moves */
	if (depth > 0)
		getAllLegalMoves(board, currentColor, rep, 1, 0);
	else
		getAllLegalMoves(board, currentColor, rep, 1, 1);/* depth == 0 check if leaf */
	amountOfMoves = rep->size;


	/* allocate current node */
	root = allocWrapper(calloc(1, sizeof(treeNode)), "calloc");
	nodeNum++;



	/* attach this node to his father */
	if (father != NULL)
		father->branch[whichSon] = root;

	/* attach the main root to gameTree */
	if (gameTree == NULL)
		gameTree = root;

	/* how we got to this node */

	root->move = allocWrapper(malloc(STRING_SIZE * sizeof(char)), "malloc");

	if (curMove != NULL)
		strcpy(root->move, curMove);

	root->boardValue = 0; /* for now, minimax will change this */
	if (depth == 0)
		root->boardValue = boardScore(board, PlayerA); /* give actual value to leaves */
	root->branch = NULL; /* start with no kids */
	root->childAmount = 0;

	if (amountOfMoves == 0) {
		if (currentColor == user_color)
		if (minimaxDepth == -1)
			root->boardValue = 10000 + depth;
		else
			root->boardValue = SCORE_K + depth;
		else
		if (minimaxDepth == -1)
			root->boardValue = -10000;
		else
			root->boardValue = -SCORE_K;

	}
	if (nodeNum >= MAX_NODES && minimaxDepth == -1){
		freeRepository(rep);
		return 0;
	}

	/* if there are more moves to be added and room in the tree */
	if ((depth > 0) && (amountOfMoves > 0)){

		root->branch = allocWrapper(calloc(amountOfMoves, sizeof(treeNode *)), "calloc");
		/* Initialize all children to null as default */
		for (i = 0; i < amountOfMoves; i++)
			root->branch[i] = NULL;
		root->childAmount = amountOfMoves;
		root->boardValue = boardScore(board, PlayerA);

		/* if there is only one option */
		if ((depth == initDepth) && (amountOfMoves == 1)){
			son = allocWrapper(calloc(1, sizeof(treeNode)), "calloc");
			son->move = allocWrapper(malloc(STRING_SIZE * sizeof(char)), "malloc");
			strcpy(son->move, rep->legalMoves[0]);
			son->boardValue = INF;
			root->branch[0] = son;
			freeRepository(rep);
			return INF;
		}


		for (i = 0; i < amountOfMoves; i++) {

			simulateTurn(board, rep->legalMoves[i], nextBoard);


			/* recursive call*/
			temp = getBestScoreBybuildingGameTree(nextBoard, depth - 1, PlayerA, nextColor, rep->legalMoves[i],
				root, i, alpha, beta, !maximizingPlayer);


			if (maximizingPlayer){
				alpha = MAX(alpha, temp);
				root->boardValue = alpha;
				if (beta <= alpha)
					break;
			}
			else{
				beta = MIN(beta, temp);
				root->boardValue = beta;
				if (beta <= alpha)
					break;
			}
		}

		freeRepository(rep);
		if (maximizingPlayer)
			return alpha;

		return beta;
	}
	/* if there are no more moves or we got to depth 0 */
	freeRepository(rep);
	return root->boardValue;

}




/* gets a tree and a best value , and returns a move from the first level of moves with this value */
/* (if randomly = 1 then a move will be chosen randomly, o.w the first move found will be returned) */
char* chooseMove(int bestValue, treeNode *tree, int randomly){

	int i = 0, countBestMoves = 0;
	treeNode *child;
	randomly = 0;
	while (i < tree->childAmount){
		child = tree->branch[i++];

		if (child->boardValue == bestValue){
			if (!randomly){
				return child->move;
			}
			/* counting how many options we got to choose from*/
			countBestMoves++;
		}
	}


	/* should never get here */
	fprintf(stderr, "Error in choosing computer move!\n");
	return NULL;
}







/* ****************************************************************************** */
/*                                   GUI GUI GUI                                  */
/* ****************************************************************************** */



void guiGameState(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){


	int i = 0, j = 0, k = 0;

	guiSetMainStage(board);
	/* set up game piece clip */
	for (i = 0; i< 2; i++){
		for (j = 0; j < 6; j++){
			clip[k].x = j * 60;
			clip[k].y = i * 60;
			clip[k].w = 60;
			clip[k].h = 60;
			k++;
		}
	}



	setBoard(board, 0);
	if (LOAD != 1) /* starting new game */
		game_state(board, WHITE);
	else{ /* LOAD == 1 */
		LOAD = 0;
		game_state(board, current_player_color);/* when loading we always load users turn */
	}
}

void guiSetMainStage(char board[BOARD_SIZE][BOARD_SIZE]){
	/* surfaces: */

	SDL_Surface *background = NULL;
	SDL_Surface *quit = NULL;
	SDL_Surface *guiBoard = NULL;

	/* Load up board image */
	quit = load_image("quit.bmp", 0);
	background = load_image("mainBackground1.png", 0);
	guiBoard = load_image("board.png", 0);


	/* Apply images to screen */
	apply_surface(0, 0, background, screen, NULL);
	apply_surface(0, 0, guiBoard, screen, NULL);
	apply_surface(500, 700, quit, screen, NULL);
	setBoard(board, 0); /* convert cmd board to GUI board */

	refresh();

	/* free loaded images */
	SDL_FreeSurface(guiBoard);
	SDL_FreeSurface(background);
	SDL_FreeSurface(quit);
}

SDL_Surface *load_image(char *filename, int transparent)
{
	/* Temporary storage for the image that's loaded */
	SDL_Surface* loadedImage = NULL;

	/* The optimized image that will be used */
	SDL_Surface* optimizedImage = NULL;


	/* Load the image */
	loadedImage = IMG_Load( filename );
	if (loadedImage == NULL) {
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		freeAllRepositories();
		freeTree(gameTree);
		exit(0);
	}
	/* Create an optimized image */
	optimizedImage = SDL_DisplayFormat(loadedImage);
	if (optimizedImage == NULL){
		printf("ERROR: failed to format image: %s\n", SDL_GetError());
		SDL_FreeSurface(loadedImage);
		freeAllRepositories();
		freeTree(gameTree);
		exit(0);
	}

	/* set all white pixels to transparent, if transparent == 1 */
	if (transparent == 1){
		SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB(optimizedImage->format, 0xFF, 0, 0xFF));

	}
	/* Free the old image */
	SDL_FreeSurface(loadedImage);

	return optimizedImage;

}

void apply_surface(int x, int  y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect* clip){

	/* make temporary rectangle to hold the offsets */
	SDL_Rect offset;

	/* give offset to the rectangle */
	offset.x = x;
	offset.y = y;

	/* Blit the surface */
	SDL_BlitSurface(source, clip, destination, &offset);

}

void initWindow(){


	/*  start SDL */
	if (SDL_Init(SDL_INIT_VIDEO) == -1){
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		freeAllRepositories();
		freeTree(gameTree);
		exit(0);
	}

	/* set up screen */
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL){
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		freeAllRepositories();
		freeTree(gameTree);
		exit(0);
	}



}


void setBoard(char board[BOARD_SIZE][BOARD_SIZE], int piecesOnly){

	int i, j;
	SDL_Surface *guiBoard = NULL;

	guiBoard = load_image("board.png", 0);
	tools = load_image("sprites3.png", 1);
	if (!piecesOnly)
		apply_surface(0, 0, guiBoard, screen, NULL);

	for (i = 0; i< BOARD_SIZE; i++){
		for (j = 0; j< BOARD_SIZE; j++){

			if (board[i][j] == WHITE_B)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[8]);
			else if (board[i][j] == WHITE_R)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[10]);
			else if (board[i][j] == WHITE_K)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[6]);
			else if (board[i][j] == WHITE_N)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[9]);
			else if (board[i][j] == WHITE_P)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[11]);
			else if (board[i][j] == WHITE_Q)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[7]);

			else if (board[i][j] == BLACK_B)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[2]);
			else if (board[i][j] == BLACK_R)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[4]);
			else if (board[i][j] == BLACK_K)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[0]);
			else if (board[i][j] == BLACK_N)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[3]);
			else if (board[i][j] == BLACK_P)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[5]);
			else if (board[i][j] == BLACK_Q)
				apply_surface(i * 60, (7 - j) * 60, tools, screen, &clip[1]);
			continue;

		}
	}
	refresh();
	SDL_FreeSurface(guiBoard);
	SDL_FreeSurface(tools);

}

/* main screen
*
* TITLE
* NEW GAME
* LOAD GAME
* QUIT
*
*/
int guiSplashScreen(char board[BOARD_SIZE][BOARD_SIZE], color* current_player_color_pointer){

	int x, y;
	SDL_Event event;
	SDL_Surface *background = NULL;

	initWindow();

	/* Set window caption */
	SDL_WM_SetCaption("Chess", NULL);

	background = load_image("splashScreen.png", 0);
	apply_surface(0, 0, background, screen, NULL);
	SDL_FreeSurface(background);

	refresh();

	while (!done){

		

		while (SDL_PollEvent(&event)){

			switch (event.type){

			case SDL_QUIT: done = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				x = event.motion.x;
				y = event.motion.y;
				if ((x <= 500) && (x >= 300) && (y <= 650) && (y >= 600)){
					done = 1;
				}
				else if ((x <= 500) && (x >= 300) && (y <= 550) && (y >= 500)){

					guiLoadScreen(board, current_player_color_pointer);
					/* back from load or type screen, redraw splash screen */
					background = load_image("splashScreen.png", 0);
					apply_surface(0, 0, background, screen, NULL);
					SDL_FreeSurface(background);
					refresh();


				}
				else if ((x <= 500) && (x >= 300) && (y <= 450) && (y >= 400)){
					/* go to choose game type */
					chooseGameType(board);
					/* back from load or type screen, redraw splash screen */
					background = load_image("splashScreen.png", 0);
					apply_surface(0, 0, background, screen, NULL);
					SDL_FreeSurface(background);
					refresh();

				}

				break;
			
			default:
				
				break;


			}


		}
	}

	/* Quit SDL */

	SDL_Quit();

	return 1;

}

void drawLoadScreen (){

	int i, height;
	SDL_Surface *background = NULL;
	SDL_Surface *state = NULL;


	background = load_image("loadBackground.png", 0);
	apply_surface(0, 0, background, screen, NULL);
	SDL_FreeSurface(background);

	for (i = 0; i < SAVE_STATES; i++){
		switch (i+1){
			case 1: state = load_image("slot1.png", 0); break;
			case 2: state = load_image("slot2.png", 0); break;
			case 3: state = load_image("slot3.png", 0); break;
			case 4: state = load_image("slot4.png", 0); break;
			case 5: state = load_image("slot5.png", 0); break;
			case 6: state = load_image("slot6.png", 0); break;
			case 7: state = load_image("slot7.png", 0); break;
			case 8: state = load_image("slot8.png", 0); break;
			case 9: state = load_image("slot9.png", 0); break;
			case 10: state = load_image("slot0.png", 0); break;
		}

		if (i+1 <= 5)
			height = 100;
		else
			height = 250;

		apply_surface(50 + ((i%5)*150), height, state, screen, NULL);
		SDL_FreeSurface(state);



	}

	/*update screen */
	refresh();
}


void guiLoadScreen(char board[BOARD_SIZE][BOARD_SIZE], color* current_player_color_pointer){

	int  x, y, i = 0, height, temp;

	SDL_Event event;

	drawLoadScreen();

	while (!done){

		

		while (SDL_PollEvent(&event)){
			switch (event.type){

			case SDL_QUIT: done = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x;
				y = event.motion.y;

				for (i = 0; i < SAVE_STATES; i++){

					if (i+1 <= 5)
						height = 100;
					else
						height = 250;

					temp = 50 + ((i%5)*150);

					if ((x >= temp) && (x <= temp + 100) && (y <= height + 100) && (y >= height)){

						loadState(board, i+1, current_player_color_pointer);
						/*redraw load screen (incase of an error pop up) */
						drawLoadScreen();

					}
					else if ((x >= 100) && (x <= 200) && (y <= 750) && (y >= 700)){
						/* back to main splash screen */
						goto JUMP;
					}
				}
				break;

			default:
				break;

			}


		}
	}

JUMP:

	return;

}



/* gui, choosing load state */
void loadState(char board[BOARD_SIZE][BOARD_SIZE], int state, color* current_player_color_pointer){


	SDL_Surface *popUpError = NULL;
	char fName[11];
	fName[10] = '\0';
	/* choose what file we want to open out of the 5 */
	switch (state){
	case 1: strcpy(fName, "state1.xml"); break;
	case 2: strcpy(fName, "state2.xml"); break;
	case 3: strcpy(fName, "state3.xml"); break;
	case 4: strcpy(fName, "state4.xml"); break;
	case 5: strcpy(fName, "state5.xml"); break;
	case 6: strcpy(fName, "state6.xml"); break;
	case 7: strcpy(fName, "state7.xml"); break;
	case 8: strcpy(fName, "state8.xml"); break;
	case 9: strcpy(fName, "state9.xml"); break;
	case 10: strcpy(fName, "state0.xml"); break;
	}

	/* error handling, incase we choose a non existing file */
	popUpError = load_image("loadStateError.bmp", 0);
	apply_surface(300, 350, popUpError, screen, NULL);
	SDL_FreeSurface(popUpError);

	if (loadGame(board, fName, current_player_color_pointer) == 0){
		/*update screen */
		refresh(); /* file error has happened */

		/* Pause */
		SDL_Delay(1000);


	}
	else{


		LOAD = 1; /* we are starting game from load point, use correct player color, not white */

		guiGameState(board, *current_player_color_pointer);
	}



	return;


}

/* gui for settings in game */
void guiSettingsState(char board[BOARD_SIZE][BOARD_SIZE]){

	int x, y;
	SDL_Surface *check50 = NULL;
	SDL_Surface *check100 = NULL;
	SDL_Surface *settingsCover1 = NULL;
	SDL_Surface *settingsCover2 = NULL;
	SDL_Event event;


	SDL_Surface *settingsScreen = NULL;

	/* load images for this screen */
	check50 = load_image("check50.png", 1);
	check100 = load_image("check100.png", 1);
	settingsScreen = load_image("settingsScreen.png", 0);
	settingsCover1 = load_image("settingsCover1.png", 0);
	settingsCover2 = load_image("settingsCover2.png", 0);
	/* set up screen */

	apply_surface(0, 0, settingsScreen, screen, NULL);

	/*update screen */
	refresh();

	while (!done){


		while (SDL_PollEvent(&event)){

			switch (event.type){

			case SDL_QUIT: done = 1;

				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x;
				y = event.motion.y;

				if ((x >= 50) && (x <= 100) && (y <= 350) && (y >= 300)){
					/* difficulty 1 */
					minimaxDepth = 1;
					initDepth = minimaxDepth;
					/* tell the user what they just did */
					apply_surface(50, 300, settingsCover1, screen, NULL);
					apply_surface(50, 300, check50, screen, NULL);
					refresh();

				}
				else if ((x >= 150) && (x <= 200) && (y <= 350) && (y >= 300)){

					/* difficulty 2 */
					minimaxDepth = 2;
					initDepth = minimaxDepth;
					/* tell the user what they just did */
					apply_surface(50, 300, settingsCover1, screen, NULL);
					apply_surface(150, 300, check50, screen, NULL);						
					refresh();

				}
				else if ((x >= 250) && (x <= 300) && (y <= 350) && (y >= 300)){
					/* difficulty 3 */
					minimaxDepth = 3;
					initDepth = minimaxDepth;
					/* tell the user what they just did */
					apply_surface(50, 300, settingsCover1, screen, NULL);
					apply_surface(250, 300, check50, screen, NULL);
					refresh();

				}
				else if ((x >= 350) && (x <= 400) && (y <= 350) && (y >= 300)){
					/* difficulty 4 */
					minimaxDepth = 4;
					initDepth = minimaxDepth;
					/* tell the user what they just did */
					apply_surface(50, 300, settingsCover1, screen, NULL);
					apply_surface(350, 300, check50, screen, NULL);
					refresh();

				}
				else if ((x >= 150) && (x <= 300) && (y <= 450) && (y >= 400)){
					/* difficulty best */
					minimaxDepth = -1;
					/* tell the user what they just did */
					apply_surface(50, 300, settingsCover1, screen, NULL);
					apply_surface(200, 400, check50, screen, NULL);
					refresh();

				}
				else if ((x >= 100) && (x <= 350) && (y <= 550) && (y >= 500)){
					/* start game */

					SDL_FreeSurface(check50);
					SDL_FreeSurface(check100);
					SDL_FreeSurface(settingsScreen);
					SDL_FreeSurface(settingsCover1);
					SDL_FreeSurface(settingsCover2);
					guiGameState(board, WHITE);
				}
				else if ((x >= 150) && (x <= 250) && (y <= 750) && (y >= 700)){
					/* back to main splash screen */
					goto JUMP2;
				}
				else if ((x >= 450) && (x <= 550) && (y <= 350) && (y >= 300)){
					/* choose white color */
					user_color = WHITE;
					/* tell the user what they just did */
					apply_surface(450, 300, settingsCover2, screen, NULL);
					apply_surface(450, 300, check100, screen, NULL);
					refresh();


				}
				else if ((x >= 650) && (x <= 750) && (y <= 350) && (y >= 300)){
					/* choose black color */
					user_color = BLACK;
					/* tell the user what they just did */
					apply_surface(450, 300, settingsCover2, screen, NULL);
					apply_surface(650, 300, check100, screen, NULL);
					refresh();


				}

				break;

			default:
				break;

			}


		}
	}

JUMP2:
	SDL_FreeSurface(check50);
	SDL_FreeSurface(check100);
	SDL_FreeSurface(settingsScreen);
	SDL_FreeSurface(settingsCover1);
	SDL_FreeSurface(settingsCover2);

	return;


}





/* flip screen */
void refresh(){
	if (SDL_Flip(screen) == -1){
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		freeAllRepositories();
		freeTree(gameTree);
		exit(0);
	}

}

/* gui for choosing amount of players */
void chooseGameType(char board[BOARD_SIZE][BOARD_SIZE]){

	int x, y;

	SDL_Event event;

	SDL_Surface *title = NULL;

	SDL_Surface *background = NULL;
	SDL_Surface *back = NULL;
	SDL_Surface *singlePlayer = NULL;
	SDL_Surface *twoPlayers = NULL;

	/* load images for this screen */
	title = load_image("gameTypeTitle.bmp", 0);

	background = load_image("mainBackground.png", 0);
	back = load_image("back.bmp", 0);
	singlePlayer = load_image("singlePlayer.bmp", 0);
	twoPlayers = load_image("twoPlayers.bmp", 0);



	/* set up screen */
	apply_surface(0, 0, background, screen, NULL);
	apply_surface(200, 300, singlePlayer, screen, NULL);
	apply_surface(200, 400, twoPlayers, screen, NULL);

	apply_surface(200, 700, back, screen, NULL);
	apply_surface(0, 100, title, screen, NULL);

	SDL_FreeSurface(back);
	SDL_FreeSurface(singlePlayer);
	SDL_FreeSurface(title);
	SDL_FreeSurface(twoPlayers);
	SDL_FreeSurface(background);



	init_board_(board);

	/*update screen */
	refresh();

	while (!done){
		
		while (SDL_PollEvent(&event)){
			switch (event.type){

			case SDL_QUIT: done = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x;
				y = event.motion.y;

				if ((x >= 200) && (x <= 600) && (y <= 350) && (y >= 300)){
					/* AI mode */
					game_mode = 2;

					guiSettingsState(board);

					/*reset up screen , we came back from settings screen*/
					title = load_image("gameTypeTitle.bmp", 0);
					background = load_image("mainBackground.png", 0);
					back = load_image("back.bmp", 0);
					singlePlayer = load_image("singlePlayer.bmp", 0);
					twoPlayers = load_image("twoPlayers.bmp", 0);

					apply_surface(0, 0, background, screen, NULL);
					apply_surface(200, 300, singlePlayer, screen, NULL);
					apply_surface(200, 400, twoPlayers, screen, NULL);
					apply_surface(200, 700, back, screen, NULL);
					apply_surface(0, 100, title, screen, NULL);

					SDL_FreeSurface(back);
					SDL_FreeSurface(singlePlayer);
					SDL_FreeSurface(title);
					SDL_FreeSurface(twoPlayers);
					SDL_FreeSurface(background);
				
					/*update screen */
					refresh();

				}
				else if ((x >= 200) && (x <= 600) && (y <= 450) && (y >= 400)){
					/* UI mode */
					game_mode = 1;

					guiGameState(board, WHITE);
				}


				else if ((x >= 200) && (x <= 300) && (y <= 750) && (y >= 700)){
					/* back to main splash screen */
					goto JUMP3;
				}

				break;

			default:
				break;
			}

		}
	}

JUMP3:


	return;

}



/* leaf gui function, prints out winner */
void guiWinMessage(color c, char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){

	int x, y;

	SDL_Event event; /* event handling */
	SDL_Surface *blackWinner = NULL;
	SDL_Surface *whiteWinner = NULL;

	blackWinner = load_image("blackWinner.bmp", 0);
	whiteWinner = load_image("whiteWinner.bmp", 0);


	if (c == BLACK)
		apply_surface(50, 550, blackWinner, screen, NULL);
	else
		apply_surface(50, 550, whiteWinner, screen, NULL);

	SDL_FreeSurface(whiteWinner);
	SDL_FreeSurface(blackWinner);

	/*update screen */
	refresh();

	while (!done){
		
		while (SDL_PollEvent(&event)){
			switch (event.type){
			case SDL_QUIT: done = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x;
				y = event.motion.y;

				if ((x >= 550) && (x <= 750) && (y <= 150) && (y >= 50)){


					/* go to main menu */

					SDL_Quit();
					freeAllRepositories();
					freeTree(gameTree);
					main(0, NULL); /*no need to reset GUI flag, already 1 */
					
					/*update screen */
					refresh();

				}
				else if ((x >= 550) && (x <= 750) && (y <= 300) && (y >= 200)){
					/* save game */
					/* get file name by giving user option to choose */
					saveState(board, current_player_color);
					guiSetMainStage(board);
					setBelowBoard(current_player_color);
					/*setBoard(board, 0);*/

					blackWinner = load_image("blackWinner.bmp", 0);
					whiteWinner = load_image("whiteWinner.bmp", 0);


					if (c == BLACK)
						apply_surface(50, 550, blackWinner, screen, NULL);
					else
						apply_surface(50, 550, whiteWinner, screen, NULL);

					SDL_FreeSurface(whiteWinner);
					SDL_FreeSurface(blackWinner);


					/*update screen */
					refresh();

				}
				else if ((x >= 550) && (x <= 750) && (y <= 450) && (y >= 350)){
					/* restart game */

					freeAllRepositories();
					freeTree(gameTree);
					init_board_(board);
					setBoard(board, 0);

					game_state(board, WHITE);

					/*update screen */
					refresh();

				}
				else if ((x >= 500) && (x <= 700) && (y <= 750) && (y >= 700)){
					/* quit game */

					done = 1;
					freeAllRepositories(); /* possible way to exit program */
					freeTree(gameTree);

				}

				break;

			default:
				break;

			}

		}
	}

	SDL_Quit();


}


void saveState(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){

	/* get slot choice from user */
	int slot = guiGetSaveSlot();
	char fName[11];
	fName[10] = '\0';

	/* choose what file we want to open out of the 5 */
	switch (slot){
		case 1: strcpy(fName, "state1.xml"); break;
		case 2: strcpy(fName, "state2.xml"); break;
		case 3: strcpy(fName, "state3.xml"); break;
		case 4: strcpy(fName, "state4.xml"); break;
		case 5: strcpy(fName, "state5.xml"); break;
		case 6: strcpy(fName, "state6.xml"); break;
		case 7: strcpy(fName, "state7.xml"); break;
		case 8: strcpy(fName, "state8.xml"); break;
		case 9: strcpy(fName, "state9.xml"); break;
		case 10: strcpy(fName,"state0.xml"); break;

		case -1:setBoard(board, 0); return; /* back was clicked, dont save */
	}

	saveGame(board, fName, current_player_color);

}

void drawSaveScreen (){

	int i, height;
	SDL_Surface *background = NULL;

	SDL_Surface *state = NULL;

	SDL_Surface *back = NULL;


	background = load_image("SaveBackground.png", 0);
	back = load_image("back.bmp", 0);


	apply_surface(0, 0, background, screen, NULL);
	apply_surface(600, 700, back, screen, NULL);

	SDL_FreeSurface(background);
	SDL_FreeSurface(back);

	for (i = 0; i < SAVE_STATES; i++){
		switch (i+1){
			case 1: state = load_image("slot1.png", 0); break;
			case 2: state = load_image("slot2.png", 0); break;
			case 3: state = load_image("slot3.png", 0); break;
			case 4: state = load_image("slot4.png", 0); break;
			case 5: state = load_image("slot5.png", 0); break;
			case 6: state = load_image("slot6.png", 0); break;
			case 7: state = load_image("slot7.png", 0); break;
			case 8: state = load_image("slot8.png", 0); break;
			case 9: state =  load_image("slot9.png", 0); break;
			case 10:state = load_image("slot0.png", 0); break;
		}

		if (i+1 <= 5)
			height = 100;
		else
			height = 250;

		apply_surface(50 + ((i%5)*150), height, state, screen, NULL);
		SDL_FreeSurface(state);

	}

	/*update screen */
	refresh();
}

int guiGetSaveSlot(){

	SDL_Surface *saveSuccessful = NULL;
	SDL_Event event;
	int x, y, i = 0, height, temp;

	saveSuccessful = load_image("saveSuccessful.bmp", 0);

	drawSaveScreen();

	/*update screen */
	refresh();

	while (!done){

		while (SDL_PollEvent(&event)){
			switch (event.type){

			case SDL_QUIT:
				done = 1;
				SDL_FreeSurface(saveSuccessful);
				SDL_Quit();
				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x;
				y = event.motion.y;

				for (i = 0; i < SAVE_STATES; i++){

					if (i+1 <= 5)
						height = 100;
					else
						height = 250;

					temp = 50+((i%5)*150);

					if ((x >= temp) && (x <= temp+100) && (y <= height + 100) && (y >= height)){
						apply_surface(150, 150, saveSuccessful, screen, NULL);
						refresh();
						SDL_Delay(1000);

						SDL_FreeSurface(saveSuccessful);
						return ((i+1)%10); /* Fifth slot chosen */
					}
					else if ((x >= 600) && (x <= 700) && (y <= 750) && (y >= 700)){
						SDL_FreeSurface(saveSuccessful);
						return -1; /* back was pressed, no saving */
					}
				}

				break;

			default:
				break;

			}
		}
	}
	return -1;  /* never gets here */

}

/* gets move from user clicks, return char* of move
* returns NULL on quit
*/
char * guiGetMove(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){


	SDL_Surface *checkerChosen = NULL;
	SDL_Event event;
	int toMove = 0, x, y, pos_x, pos_y, pos_w, pos_z;
	char *move = allocWrapper(calloc(20, sizeof(char)), "calloc");


	strcpy(move, "move <x,y> to <x,y>");
	
	/* Update screen */
	refresh();

	while (!done){

		while (SDL_PollEvent(&event)){
			switch (event.type){

			case SDL_QUIT:
				/*freeAllRepositories();
				freeTree(gameTree);
				gameTree = NULL; */
				SDL_Quit();
				done = 1;

				/*exit(0); */
				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x;
				y = event.motion.y;
				if ((x <= 480) && (y <= 480) && (x > 0) && (y > 0)){ /* click inside board borders */
					pos_x = 8 - (int)((480 - x) / 60);
					pos_y = 1 + (int)((480 - y) / 60);
					toMove = (toMove + 1) % 2;
					if (toMove == 0){
						/* even amount of clicks, try to make it into a move */
						move[6] = (char)(48 + pos_w + 48); /* from */
						move[8] = (char)(48 + pos_z);
						move[15] = (char)(48 + pos_x + 48); /* to */
						move[17] = (char)(48 + pos_y);

						setBoard(board, 0);

						return move;


					}
					pos_w = pos_x;/* save odd click location */
					pos_z = pos_y;

					checkerChosen = load_image("checkerChosen.png", 0);
					apply_surface(60 * (pos_w - 1), 60 * (8 - pos_z), checkerChosen, screen, NULL);
					setBoard(board, 1); /* set only pieces, not board itself */
					SDL_FreeSurface(checkerChosen);
					/* Update screen */
					refresh();
				}

				/* click outside of board borders */
				else if ((x >= 550) && (x <= 750) && (y <= 150) && (y >= 50)){
					/* go to main menu */

					/*freeAllRepositories(); */
					SDL_Quit();
					main(0, NULL); /* no need to reset gui flag, already 1 */
				}
				else if ((x >= 550) && (x <= 750) && (y <= 300) && (y >= 200)){
					/* save game */
					/* get file name by giving user option to choose */


					saveState(board, current_player_color);
					guiSetMainStage(board); /* reset screen after save screen */
					setBelowBoard(current_player_color);
					/* Update screen */
					refresh();


				}
				else if ((x >= 550) && (x <= 750) && (y <= 450) && (y >= 350)){
					/* restart game */

					freeAllRepositories();
					init_board_(board);
					setBoard(board, 0);

					game_state(board, WHITE);
					/* Update screen */
					refresh();


				}
				else if ((x >= 500) && (x <= 700) && (y <= 750) && (y >= 700)){
					/* quit game */

					SDL_Quit();
					done = 1;


				}
				break;
			default:
				break;
			}
		}
	}


	free(move); move = NULL;
	return NULL; /* we hit quit */


}

void moveError(){

	SDL_Surface *illegalMove = NULL;

	illegalMove = load_image("illegalMove.bmp", 0);

	apply_surface(50, 550, illegalMove, screen, NULL);

	SDL_FreeSurface(illegalMove);
	refresh();
	SDL_Delay(1000);
	return;


}

char * choosePromotion(){


	char *result = allocWrapper(calloc(10, sizeof(char)), "calloc");
	SDL_Surface *background = NULL;
	SDL_Event event;
	int x, y;


	tools = load_image("sprites3.png", 1);
	background = load_image("mainBackground.png", 0);

	apply_surface(0, 0, background, screen, NULL);
	apply_surface(300, 300, tools, screen, &clip[3]);/* knight */
	apply_surface(500, 300, tools, screen, &clip[4]);/* rook */
	apply_surface(300, 500, tools, screen, &clip[2]);/* bishop */
	apply_surface(500, 500, tools, screen, &clip[1]);/* queen */

	SDL_FreeSurface(tools);
	SDL_FreeSurface(background);

	/* Update screen */
	refresh();

	while (!done){

		while (SDL_PollEvent(&event)){
			switch (event.type){

			case SDL_QUIT: done = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x;
				y = event.motion.y;
				if ((x >= 300) && (x <= 360) && (y <= 360) && (y >= 300)){
					/* knight */
					strcpy(result, " knight\0");
					return result;
				}
				else if ((x >= 500) && (x <= 560) && (y <= 360) && (y >= 300)){
					/* rook */
					strcpy(result, " rook\0");
					return result;
				}
				else if ((x >= 300) && (x <= 360) && (y <= 560) && (y >= 500)){
					/* bishop */
					strcpy(result, " bishop\0");
					return result;
				}
				else if ((x >= 500) && (x <= 560) && (y <= 560) && (y >= 500)){
					/* queen */
					strcpy(result, " queen\0");
					return result;
				}
				break;
			default:
				break;
			}
		}
	}
	free(result);
	return NULL; /* we hit x */

}

/* show a message in case of check */
void guiCheckMessage(char board[BOARD_SIZE][BOARD_SIZE]){

	SDL_Surface *check = NULL;

	check = load_image("checkMessage.bmp", 0);

	apply_surface(140, 140, check, screen, NULL);

	SDL_FreeSurface(check);

	refresh();
	SDL_Delay(1000);

	setBoard(board, 0);

}


void guiUserTurn(char board[BOARD_SIZE][BOARD_SIZE], color current_player_color){
	char  str[STRING_SIZE], theMove[STRING_SIZE], *temp;



	checker srcChkr;
	checker destChkr;
	legalMovesRepository re;
	legalMovesRepository* rep = &re;


	/* initialize repository */
	rep->legalMoves = NULL;
	rep->pos = 0; rep->size = 0;
	addRepToArrayOfReps(rep);

	/* creating a repository of all legal available moves */
	getAllLegalMoves(board, current_player_color, rep, 1, 0);



	while (1){ /* loop. incase of error, we get to try again */
		/* get move */
		setBelowBoard(current_player_color);
		temp = guiGetMove(board, current_player_color);
		if (temp == NULL){
			/* we got NULL, this means we hit quit or x in the getMove window */
			break;
		}
		strncpy(theMove, temp + 5, 15);
		free(temp);



		if (theMove == NULL){

			moveError();
			continue;
		}

		/* check legal source checker */
		strncpy(str, theMove, 5); /* str = source checker <x,y> */
		srcChkr = stringToChecker(str);
		if (srcChkr.y == 0){

			moveError();
			continue;
		}

		/* check legal dest checker */
		strncpy(str, theMove + 9, 5); /* str = dest checker <x,y> */
		destChkr = stringToChecker(str);
		if (destChkr.y == 0){

			moveError();
			continue;
		}

		/* checking if the user has a tool on the source checker */
		if (!(((current_player_color == WHITE) &&
			WHITE_TOOL(board[srcChkr.x - 97][srcChkr.y - 1])) ||
			((current_player_color == BLACK) &&
			BLACK_TOOL(board[srcChkr.x - 97][srcChkr.y - 1]))))
		{

			moveError(); /* no tool in source checker */
			continue;
		}

		/* getting the extra string if a pawn has to be promoted */
		if (((board[srcChkr.x - 97][srcChkr.y - 1] == WHITE_P && destChkr.y == BOARD_SIZE) ||
			(board[srcChkr.x - 97][srcChkr.y - 1] == BLACK_P && destChkr.y == 1)) /*&& isItInRepository(rep,theMove)*/)
		{
			/* GUI, choose what you want your pawn to be */
			temp = choosePromotion();/* gui function for choosing promotion */

			if (temp == NULL)
				break; /* x was hit in promotion screen */
			strcat(theMove, temp);
			free(temp);

			guiSetMainStage(board);
		}

		/* checking if it a legal move by searching in the repository */

		if (isItInRepository(rep, theMove) == 0){

			moveError(); /*move not in repository */
			continue;

		}

		/* the move is legal - update the board */

		simulateTurn(board, theMove, board);
		/*setBoard(board,0);*/

		/*not taking care of castling in GUI yet */


		break; /* break the while loop, we have a legal move */

	}

	freeRepository(rep);

}

void setBelowBoard(color current_player_color){

	SDL_Surface *whatDo = NULL;
	SDL_Surface *whiteTurn = NULL;
	SDL_Surface *blackTurn = NULL;


	whiteTurn = load_image("whiteTurn.bmp", 0);
	blackTurn = load_image("blackTurn.bmp", 0);

	if (current_player_color == WHITE)
		apply_surface(250, 550, whiteTurn, screen, NULL);
	else
		apply_surface(250, 550, blackTurn, screen, NULL);


	whatDo = load_image("chooseMove.bmp", 0);
	if (current_player_color == user_color)
		apply_surface(50, 550, whatDo, screen, NULL);
	else{
		SDL_FreeSurface(whatDo);
		whatDo = load_image("blank.bmp", 0);
		apply_surface(50, 550, whatDo, screen, NULL);
	}

	refresh();
	SDL_FreeSurface(whatDo);
	SDL_FreeSurface(whiteTurn);
	SDL_FreeSurface(blackTurn);

}
