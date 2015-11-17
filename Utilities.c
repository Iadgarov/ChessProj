#include "Chess.h"
#include "Def.h"


legalMovesRepository* arrayOfReps[NUM_OF_REPS];

/* *******************************************************/
/* **************  minor helping methods   ***************/
/* *******************************************************/


/*
* converts game piece to number
* pawn = 0, knight = 1, bishop = 2, rook = 3, queen = 4, king = 5
*/
int pieceToNum(char c){

	switch (c){
	case 'm': return 0;
	case 'M': return 0;
	case 'n': return 1;
	case 'N': return 1;
	case 'b': return 2;
	case 'B': return 2;
	case 'r': return 3;
	case 'R': return 3;
	case 'q': return 4;
	case 'Q': return 4;
	case 'k': return 5;
	case 'K': return 5;

	}
	return -1;
}

/*
gets a string in <x,y> format and returns a corresponding checker.
if the checker is illegal - returns <0,0> checker
*/
checker stringToChecker(char* str){
	checker c = { '0', 0 };
	char x = str[1];
	int  y = str[3] - 48;

	if (!(x<'a' || x>'h' || y<1 || y>BOARD_SIZE)){
		c.x = x;
		c.y = y;
	}
	return c;

}


/*the function cuts a substring (substr) from a given string between the indexes i and j (assuming i<=j)  */
void substringFromString(char* substr, int i, int j, char *str){
	int k = 0, dist;

	dist = j - i + 1;
	/* middle part */
	for (k = 0; k < dist; k++)
		substr[k] = str[i++];

	substr[dist] = '\0';

}


void convertMoveToString(char* str, checker srcChkr, checker dstChkr){
	str[0] = '<';
	str[1] = srcChkr.x;
	str[2] = ',';
	str[3] = srcChkr.y + 48;
	str[4] = '>';
	str[5] = ' ';
	str[6] = 't';
	str[7] = 'o';
	str[8] = ' ';
	str[9] = '<';
	str[10] = dstChkr.x;
	str[11] = ',';
	str[12] = dstChkr.y + 48;
	str[13] = '>';
	str[14] = '\0';
}

void convertCheckerToString(char* str, checker chkr){
	str[0] = '<';
	str[1] = chkr.x;
	str[2] = ',';
	str[3] = chkr.y + 48;
	str[4] = '>';
	str[5] = '\0';
}



/* *******************************************************/
/* ****************  repository methods   ****************/
/* *******************************************************/

void addPromotionMoveToRep(char *str, legalMovesRepository* rep){

	char str2[STRING_SIZE];

	addMoveToRepository(rep, str); /* Default promotion */

	/* promoting to knight */
	strcpy(str2, str);
	strcat(str2, " knight");
	addMoveToRepository(rep, str2);

	/* promoting to bishop */
	strcpy(str2, str);
	strcat(str2, " bishop");
	addMoveToRepository(rep, str2);

	/* promoting to rook */
	strcpy(str2, str);
	strcat(str2, " rook");
	addMoveToRepository(rep, str2);

	/* promoting to queen */
	strcpy(str2, str);
	strcat(str2, " queen");
	addMoveToRepository(rep, str2);

}

/* the function gets a move string and adds it to the repository of legal moves*/
void addMoveToRepository(legalMovesRepository* rep, char* str){
	char **tempMoves;

	if (rep->size == 0){
		/* making sure the legalMoves array is brand new */
		if (rep->legalMoves != NULL)
			free(rep->legalMoves);
		rep->legalMoves = allocWrapper(malloc(sizeof(char*)), "malloc");
		rep->legalMoves[rep->size] = NULL;
	}
	else{

		tempMoves = realloc(rep->legalMoves, (rep->size + 1) *sizeof(char*));
		if (tempMoves == NULL){  /* realloc was not good */
			allocWrapper(NULL, "realloc");
		}
		else{		/* realloc was good */
			rep->legalMoves = tempMoves;
			rep->legalMoves[rep->size] = NULL;
		}
	}

	rep->legalMoves[rep->size] = allocWrapper(malloc(STRING_SIZE * sizeof(char)), "malloc");
	strcpy(rep->legalMoves[rep->size], str);

	/* repository is being incremented only once we are sure the alloc was successful !!! */
	rep->size++;

}


/* returns 1 if the string is in the repository. 0  - o.w.*/
int isItInRepository(legalMovesRepository* rep, char* str){
	int i;
	for (i = 0; i < rep->size; i++)
	if (strcmp(str, rep->legalMoves[i]) == 0)
		return 1;
	return 0;
}


/* frees the allocated memory of the repository */
void freeRepository(legalMovesRepository* repository){
	int i;
	removeRepFromArrayOfReps(repository);

	/* happens only when there was a problem with realloc, and the legalMoves array was already freed by realloc */
	if (repository->legalMoves == NULL)
		return;

	/* free space of strings in repository*/
	for (i = 0; i < repository->size; i++){
		if (repository->legalMoves[i] != NULL)
			free(repository->legalMoves[i]);
	}

	/* free the space of pointers to string in repository*/
	if (repository->size > 0)
		free(repository->legalMoves);

}


/* frees the allocated memory of all repositories */
void freeAllRepositories(){
	int i;

	/* free space of strings in repository*/
	for (i = 0; i < NUM_OF_REPS; i++)
	if (arrayOfReps[i] != NULL)
		freeRepository(arrayOfReps[i]);

}


/* initializes all pointers to NULL */
void initArrayOfReps(){
	int i;
	for (i = 0; i < NUM_OF_REPS; i++)
		arrayOfReps[i] = NULL;
}

/* gets a repository and adding it to the array in an empty spot. returns the position in the array */
int addRepToArrayOfReps(legalMovesRepository* rep){
	int i;
	for (i = 0; i < NUM_OF_REPS; i++){
		if (arrayOfReps[i] == NULL){
			arrayOfReps[i] = rep;
			rep->pos = i;
			return i;
		}
	}
	fprintf(stderr, "too many repositories");
	return -1;
}

/* removing rep from the array of repositories*/
void removeRepFromArrayOfReps(legalMovesRepository* rep){
	arrayOfReps[rep->pos] = NULL;
}

/***************************************************/


/* frees all the memory allocated for this tree */
void freeTree(treeNode *tree){
	int i;


	if (tree == NULL)
		return;
	if (tree->branch != NULL){
		for (i = 0; i< tree->childAmount; i++)
		if (tree->branch[i] != NULL)
			freeTree(tree->branch[i]);

		free(tree->branch);
	}
	free(tree->move);
	free(tree);


}



/* *******************************************************/
/* ********************  wrappers   **********************/
/* *******************************************************/

/* closes the program properly when there is a scanf error */
void scanfWrapper(int num){
	if (num != 1){
		fprintf(stderr, "Error: standard function scanf has failed\n");
		freeAllRepositories();
		freeTree(gameTree);
		exit(0);
	}
}

/* closes the program properly when there is an allocation error */
void* allocWrapper(void* ptr, char* allocName){

	if (ptr == NULL){
		fprintf(stderr, "Error: standard function %s has failed\n", allocName);
		freeAllRepositories();
		freeTree(gameTree);


		exit(0);
	}
	return ptr;
}

/* ****************************************************** */
/* ***************  board methods   ********************* */
/* ****************************************************** */

/*
the function prints a line
*/
void print_line(){
	int i;
	printf(" |");
	for (i = 1; i < BOARD_SIZE * 4; i++){
		printf("-");
	}
	printf("|\n");
}

/*
the function gets a board and prints it.
*/
void print_board_(char board[BOARD_SIZE][BOARD_SIZE])
{
	int i, j;
	print_line();
	for (j = BOARD_SIZE - 1; j >= 0; j--)
	{
		printf("%d", j + 1);
		for (i = 0; i < BOARD_SIZE; i++){
			printf("| %c ", board[i][j]);
		}
		printf("|\n");
		print_line();
	}
	printf("  ");
	for (j = 0; j < BOARD_SIZE; j++){
		printf(" %c  ", (char)('a' + j));
	}
	printf("\n");
}


/*
the function gets a board and initializes it as a normal chess game.
*/
void init_board_(char board[BOARD_SIZE][BOARD_SIZE]){

	int i, j;


	board[0][0] = WHITE_R;
	board[1][0] = WHITE_N;
	board[2][0] = WHITE_B;
	board[3][0] = WHITE_Q;
	board[4][0] = WHITE_K;
	board[5][0] = WHITE_B;
	board[6][0] = WHITE_N;
	board[7][0] = WHITE_R;

	board[0][7] = BLACK_R;
	board[1][7] = BLACK_N;
	board[2][7] = BLACK_B;
	board[3][7] = BLACK_Q;
	board[4][7] = BLACK_K;
	board[5][7] = BLACK_B;
	board[6][7] = BLACK_N;
	board[7][7] = BLACK_R;

	for (i = 0; i < BOARD_SIZE; i++){
		board[i][1] = WHITE_P;
		board[i][6] = BLACK_P;
	}



	for (j = 2; j < BOARD_SIZE - 2; j++){
		for (i = 0; i < BOARD_SIZE; i++){
			board[i][j] = EMPTY;
		}
	}

}

/*
the function gets a board and clears it.
*/
void clear_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			board[i][j] = EMPTY;
		}
	}
}


/* copy an old board into a new one - making them identical*/
void copyBoard(char oldBoard[BOARD_SIZE][BOARD_SIZE], char newBoard[BOARD_SIZE][BOARD_SIZE]){
	int i, j;

	for (i = 0; i < BOARD_SIZE; i++)
	for (j = 0; j < BOARD_SIZE; j++)
		newBoard[i][j] = oldBoard[i][j];
}

/* gets a board and a color of player, and returns the score of the board from the point of view of this player*/
int boardScore(char board[BOARD_SIZE][BOARD_SIZE], color curColor){

	int i, j;
	int whiteScore = 0;
	int blackScore = 0;

	if (minimaxDepth == -1)
		return bestBoardScore(board, curColor);

	for (i = 0; i < BOARD_SIZE; i++)
	for (j = 0; j < BOARD_SIZE; j++){

		if (board[i][j] == WHITE_P)
			whiteScore += SCORE_P;
		else if (board[i][j] == WHITE_N)
			whiteScore += SCORE_N;
		else if (board[i][j] == WHITE_B)
			whiteScore += SCORE_B;
		else if (board[i][j] == WHITE_R)
			whiteScore += SCORE_R;
		else if (board[i][j] == WHITE_Q)
			whiteScore += SCORE_Q;
		else if (board[i][j] == WHITE_K)
			whiteScore += SCORE_K;
		else if (board[i][j] == BLACK_P)
			blackScore += SCORE_P;
		else if (board[i][j] == BLACK_N)
			blackScore += SCORE_N;
		else if (board[i][j] == BLACK_B)
			blackScore += SCORE_B;
		else if (board[i][j] == BLACK_R)
			blackScore += SCORE_R;
		else if (board[i][j] == BLACK_Q)
			blackScore += SCORE_Q;
		else if (board[i][j] == BLACK_K)
			blackScore += SCORE_K;

	}




	if (curColor == BLACK)
		return (blackScore - whiteScore);
	return (whiteScore - blackScore);




}

int bestBoardScore(char board[BOARD_SIZE][BOARD_SIZE], color curColor){

	int i, j, temp1, temp2, count = 0;
	int whiteScore = 0, blackScore = 0;
	int wKnightCount = 0, wBishopCount = 0;
	int bKnightCount = 0, bBishopCount = 0;

	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){

			if (board[i][j] == EMPTY)
				continue;

			if (WHITE_TOOL(board[i][j])){
				whiteScore += piece_value[pieceToNum(board[i][j])];
				if (board[i][j] == WHITE_B)
					wBishopCount += 1;
				if (board[i][j] == WHITE_N)
					wKnightCount += 1;
			}
			else{
				blackScore += piece_value[pieceToNum(board[i][j])];
				if (board[i][j] == BLACK_B)
					bBishopCount += 1;
				if (board[i][j] == BLACK_N)
					bKnightCount += 1;
			}
			count++;

		}
	}

	temp1 = whiteScore;
	temp2 = blackScore;



	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){

			if (board[i][j] == EMPTY)
				continue;

			if (board[i][j] == WHITE_N){
				whiteScore += knight_score[8 * j + i];
				if (count <= 10)
					whiteScore -= 10; /* knights are worth less at endgame */
			}
			else if (board[i][j] == WHITE_B){
				whiteScore += bishop_score[8 * j + i];
				if (wBishopCount >= 2) /* two bishops are better than 1 */
					whiteScore += 10;
				if (count <= 10)
					whiteScore += 10; /*bishops are good at end game */
			}
			else if (board[i][j] == WHITE_P){
				whiteScore += pawn_score[8 * j + i];
				if (board[i][j - 1] == WHITE_P)
					whiteScore -= 10; /* double pawn penalty */
			}
			else if (board[i][j] == WHITE_K){
				if (temp2 > 1200)
					whiteScore += king_score[8 * j + i];
				else
					whiteScore += king_endgame_score[8 * j + i];
			}

			if (board[i][j] == BLACK_N){
				blackScore += knight_score[8 * j + i];
				if (count <= 10)
					blackScore -= 10; /* knights are worth less at endgame */
			}
			else if (board[i][j] == BLACK_B){
				blackScore += bishop_score[8 * j + i];

				if (bBishopCount >= 2) /* two bishops are better than 1 */
					blackScore += 10;
				if (count <= 10)
					blackScore += 10; /*bishops are good at end game */
			}
			else if (board[i][j] == BLACK_P){
				blackScore += pawn_score[8 * j + i];
				if (board[i][j + 1] == BLACK_P)
					blackScore -= 10; /* double pawn penalty */
			}
			else if (board[i][j] == BLACK_K){
				if (temp1 > 1200)
					blackScore += king_score[8 * j + i];
				else
					blackScore += king_endgame_score[8 * j + i];
			}



		}
	}

	if (curColor == BLACK)
		return (blackScore - whiteScore);
	return (whiteScore - blackScore);


}

/* *******************************************************/
/* ***************  moves methods   **********************/
/* *******************************************************/
/*
the function gets a board , and player color
and prints all the legal moves the given player has.
*/

void getAllLegalMoves(char board[BOARD_SIZE][BOARD_SIZE], color player_color, legalMovesRepository* rep, int checkThreat, int depthFlag){
	int i, j;
	checker srcChkr /*, rookChkr*/;

	/* initializing the repository */
	rep->size = 0;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if ((player_color == BLACK && BLACK_TOOL(board[i][j])) ||
				(player_color == WHITE && WHITE_TOOL(board[i][j]))){
				srcChkr.x = i + 97;
				srcChkr.y = j + 1;

				/* go through all the possible checkers on board and try them as a destination*/
				getLegalMoves(board, player_color, srcChkr, rep, checkThreat);
				if (depthFlag == 1){
					if ((rep->size) > 0) /* found first legal move, not a leaf */
						return;
				}


			}
		}
	}

}

/* sub-method of getAllLegalMoves - scans all possible destinations */
void getLegalMoves(char board[BOARD_SIZE][BOARD_SIZE], color player_color, checker srcChkr, legalMovesRepository* rep, int checkThreat){
	int k = srcChkr.x - 97, l = srcChkr.y - 1;
	int i = k + l * 8, n, j;
	checker dstChkr;
	char str[STRING_SIZE];

	if ((board[k][l] == BLACK_P) || (board[k][l] == WHITE_P)){
		if (player_color == WHITE){
			if ((k != 0) && (BLACK_TOOL(board[k - 1][l + 1]))){ /* eat left */
				dstChkr.x = k - 1 + 97;
				dstChkr.y = l + 1 + 1;

				convertMoveToString(str, srcChkr, dstChkr);
				if (isItLegal(board, str, player_color)){
					if (l == 6){ /* promotion time */
						addPromotionMoveToRep(str, rep);
					}
					else /* regular move */
						addMoveToRepository(rep, str);
				}

			}
			if ((k != 7) && (BLACK_TOOL(board[k + 1][l + 1]))){ /* eat right */
				dstChkr.x = k + 1 + 97;
				dstChkr.y = l + 1 + 1;
				/* Check legality */

				convertMoveToString(str, srcChkr, dstChkr);
				if (isItLegal(board, str, player_color)){
					if (l == 6){ /* promotion time */
						addPromotionMoveToRep(str, rep);
					}
					else /* regular move */
						addMoveToRepository(rep, str);
				}

			}
			if (board[k][l + 1] == EMPTY){ /* walk forward */
				dstChkr.x = k + 97;
				dstChkr.y = l + 1 + 1;
				/* Check legality */

				convertMoveToString(str, srcChkr, dstChkr);
				if (isItLegal(board, str, player_color)){
					if (l == 6){ /* promotion time */
						addPromotionMoveToRep(str, rep);
					}
					else /* regular move */
						addMoveToRepository(rep, str);
				}


			}
		}
		else{
			if ((k != 0) && (WHITE_TOOL(board[k - 1][l - 1]))){ /* eat left */
				dstChkr.x = k - 1 + 97;
				dstChkr.y = l - 1 + 1;
				/* Check legality */

				convertMoveToString(str, srcChkr, dstChkr);
				if (isItLegal(board, str, player_color)){
					if (l == 1){ /* promotion time */
						addPromotionMoveToRep(str, rep);
					}
					else /* regular move */
						addMoveToRepository(rep, str);
				}
			}
			if ((k != 7) && (WHITE_TOOL(board[k + 1][l - 1]))){ /* eat right */
				dstChkr.x = k + 1 + 97;
				dstChkr.y = l - 1 + 1;
				/* Check legality */

				convertMoveToString(str, srcChkr, dstChkr);
				if (isItLegal(board, str, player_color)){
					if (l == 1){ /* promotion time */
						addPromotionMoveToRep(str, rep);
					}
					else /* regular move */
						addMoveToRepository(rep, str);
				}

			}
			if (board[k][l - 1] == EMPTY){ /* walk forward */
				dstChkr.x = k + 97;
				dstChkr.y = l - 1 + 1;
				/* Check legality */

				convertMoveToString(str, srcChkr, dstChkr);
				if (isItLegal(board, str, player_color)){
					if (l == 1){ /* promotion time */
						addPromotionMoveToRep(str, rep);
					}
					else /* regular move */
						addMoveToRepository(rep, str);
				}


			}
		}

	}
	else{ /* not pawns */
		for (j = 0; j < offsets[pieceToNum(board[k][l])]; ++j)
		for (n = i;;){
			n = mailbox[mailbox64[n] + offset[pieceToNum(board[k][l])][j]];
			dstChkr.x = (n % 8) + 97;
			dstChkr.y = (int)(n / 8) + 1;
			convertMoveToString(str, srcChkr, dstChkr);
			if (n == -1)
				break; /*moved outside of board */
			if (board[dstChkr.x - 97][dstChkr.y - 1] != EMPTY){
				if ((player_color == WHITE && BLACK_TOOL(board[dstChkr.x - 97][dstChkr.y - 1])) ||
					(player_color == BLACK && WHITE_TOOL(board[dstChkr.x - 97][dstChkr.y - 1]))){
					/* check move legality */
					if (isItLegal(board, str, player_color))
						addMoveToRepository(rep, str); /* eating move */
				}
				break;
			}
			/* check move legality */
			if (isItLegal(board, str, player_color))
				addMoveToRepository(rep, str); /* none eating move */
			if (!slide[pieceToNum(board[k][l])]) /* if it doesnt slide */
				break;
		}


	}


}

/* is the king safe after i did a move */
int isItLegal(char board[BOARD_SIZE][BOARD_SIZE], char *move, color c){
	char tempBoard[BOARD_SIZE][BOARD_SIZE];
	int temp;
	simulateTurn(board, move, tempBoard);
	temp = isKingSafe(tempBoard, c);

	return temp;

}
/* is king of color c safe */
int isKingSafe(char board[BOARD_SIZE][BOARD_SIZE], color c){
	int i = 0, j = 0;
	for (i = 0; i < BOARD_SIZE; i++)
	for (j = 0; j< BOARD_SIZE; j++){
		if ((c == BLACK) && (board[i][j] == BLACK_K))
			return !underAttack(i, j, board, OPPOSITE_COLOR(c));
		if ((c == WHITE) && (board[i][j] == WHITE_K))
			return !underAttack(i, j, board, OPPOSITE_COLOR(c));
	}
	return 1; /* never gets here */

}

/* is said square under attack by color attacker */
int underAttack(int i, int j, char board[BOARD_SIZE][BOARD_SIZE], color attacker){
	int k, l, n, m;

	for (k = 0; k < BOARD_SIZE; k++)
	for (l = 0; l< BOARD_SIZE; l++){
		if (((attacker == BLACK) && (BLACK_TOOL(board[k][l]))) ||
			((attacker == WHITE) && (WHITE_TOOL(board[k][l])))){

			if ((board[k][l] == BLACK_P) || (board[k][l] == WHITE_P))
			if (attacker == WHITE){
				if ((k != 0) && (k - 1 == i) && (l == j - 1))
					return 1;
				if ((k != 7) && (k + 1 == i) && (l == j - 1))
					return 1;
			}
			else{
				if ((k != 0) && (k - 1 == i) && (l == j + 1))
					return 1;
				if ((k != 7) && (k + 1 == i) && (l == j + 1))
					return 1;


			}
			else{ /* not pawns */
				for (m = 0; m < offsets[pieceToNum(board[k][l])]; ++m)
				for (n = l * 8 + k;;){
					n = mailbox[mailbox64[n] + offset[pieceToNum(board[k][l])][m]];


					if (n == -1)
						break; /*moved outside of board */
					if ((n % 8 == i) && ((int)(n / 8) == j))
						return 1;
					if (board[n % 8][(int)(n / 8)] != EMPTY)
						break;

					if (!slide[pieceToNum(board[k][l])]) /* if it doesnt slide */
						break;
				}
			}
		}
	}
	return 0;

}

/* *******************************************************/
/* *****************  file methods   *********************/
/* *******************************************************/
/*
the function gets a board, a file name and the color of the current player
and saves the game into the given file.
It returns 1 if the file has been saved successfully, o.w. 0.

*/
int saveGame(char board[BOARD_SIZE][BOARD_SIZE], char* filename, color current_player_color){
	FILE *filePointer = NULL;
	char ch, stringToWrite[STRING_SIZE];
	int l, i, j;

	/* trying to open the file */
	filePointer = fopen(filename, "w");

	if (filePointer == NULL)
		return 0;

	/* file has been opened successfuly*/

	if (fprintf(filePointer, "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n") < 0){
		fclose(filePointer);
		return 0;
	}
	if (fprintf(filePointer, "<game>\n") < 0){
		fclose(filePointer);
		return 0;
	}

	/* saving type of game */
	strcpy(stringToWrite, "\t<type>");
	l = strlen(stringToWrite);
	stringToWrite[l] = game_mode + 48;
	stringToWrite[l + 1] = '\0';
	strcat(stringToWrite, "</type>\n");
	if (fprintf(filePointer, "%s", stringToWrite) < 0){
		fclose(filePointer);
		return 0;
	}
	if (game_mode == 2) {

		/* saving difficulty of game */
		strcpy(stringToWrite, "\t<difficulty>");
		l = strlen(stringToWrite);
		if (minimaxDepth == -1)
			strcat(stringToWrite, "Best");
		else{
			stringToWrite[l] = minimaxDepth + 48;
			stringToWrite[l + 1] = '\0';
		}

		strcat(stringToWrite, "</difficulty>\n");
		if (fprintf(filePointer, "%s", stringToWrite) < 0){
			fclose(filePointer);
			return 0;
		}

		/* saving user_color */
		strcpy(stringToWrite, "\t<user_color>");
		if (user_color == WHITE)
			strcat(stringToWrite, "White");
		else{
			strcat(stringToWrite, "Black");
		}

		strcat(stringToWrite, "</user_color>\n");
		if (fprintf(filePointer, "%s", stringToWrite) < 0){
			fclose(filePointer);
			return 0;
		}
	}

	/* saving color of current player */
	strcpy(stringToWrite, "\t<next_turn>");
	if (current_player_color == WHITE)
		strcat(stringToWrite, "White");
	else{
		strcat(stringToWrite, "Black");
	}

	strcat(stringToWrite, "</next_turn>\n");
	if (fprintf(filePointer, "%s", stringToWrite) < 0){
		fclose(filePointer);
		return 0;
	}

	/* saving the board */
	if (fprintf(filePointer, "\t<board>\n") < 0){
		fclose(filePointer);
		return 0;
	}


	for (i = 0; i < BOARD_SIZE; i++){
		/* saving the i-th row */
		strcpy(stringToWrite, "\t\t<row_");
		l = strlen(stringToWrite);
		stringToWrite[l] = (BOARD_SIZE - i) + 48;
		stringToWrite[l + 1] = '>';
		stringToWrite[l + 2] = '\0';

		for (j = 0; j < BOARD_SIZE; j++){
			if (board[j][BOARD_SIZE - i - 1] == EMPTY)
				ch = '_';
			else
				ch = board[j][BOARD_SIZE - i - 1];

			l = strlen(stringToWrite);
			stringToWrite[l] = ch;
			stringToWrite[l + 1] = '\0';
		}


		strcat(stringToWrite, "</row_");
		l = strlen(stringToWrite);
		stringToWrite[l] = (BOARD_SIZE - i) + 48;
		stringToWrite[l + 1] = '\0';

		strcat(stringToWrite, ">\n");
		if (fprintf(filePointer, "%s", stringToWrite) < 0){
			fclose(filePointer);
			return 0;
		}
	}


	if (fprintf(filePointer, "\t</board>\n") < 0){
		fclose(filePointer);
		return 0;
	}


	/*********************/

	if (fprintf(filePointer, "</game>\n") < 0){
		fclose(filePointer);
		return 0;
	}




	fclose(filePointer);
	return 1;
}


/*
the function gets a board, a file name and a color pointer
and loads the game (current_player_color will point to the color of next player to play)
It returns 1 if the file has been loaded successfully, o.w. 0.

*/
int loadGame(char board[BOARD_SIZE][BOARD_SIZE], char* filename, color* current_player_color){
	FILE *filePointer = NULL;
	char stringToRead[STRING_SIZE], tempStr[STRING_SIZE];
	int i, j;

	/* trying to open the file */
	filePointer = fopen(filename, "r");

	if (filePointer == NULL){

		return 0;
	}

	/* file has been opened successfuly*/
	/* first two lines keep no interesting information */
	/* (<?xml version="1.0" encoding="UTF-8"?>) */
	if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
		fclose(filePointer);

		return 0;
	}

	/*  (<game>) */
	if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
		fclose(filePointer);
		return 0;
	}

	/* trying to load type of game */
	if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
		fclose(filePointer);
		return 0;
	}

	/* checking if this line is really "type" */
	substringFromString(tempStr, 2, 5, stringToRead);


	if (strcmp(tempStr, "type") == 0){

		/* stringToRead[6] can either be '1' or '2' */
		game_mode = stringToRead[7] - 48;

		if (game_mode == 2) {

			/* loading difficulty of game */
			if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
				fclose(filePointer);
				return 0;
			}

			/* checking if this line is really "difficulty" */
			substringFromString(tempStr, 2, 11, stringToRead);

			if (strcmp(tempStr, "difficulty") == 0){

				/* stringToRead[13] can either be 'B' (for best) or 1-4 (for depth) */
				if (stringToRead[13] == 'B')
					minimaxDepth = -1;
				else{
					minimaxDepth = stringToRead[13] - 48;
					initDepth = minimaxDepth;
				}
			}
			else {
				/* default value */
				minimaxDepth = 1;
				initDepth = minimaxDepth;
				fseek(filePointer, -1, SEEK_CUR);
			}

			/* loading user color */
			if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
				fclose(filePointer);
				return 0;
			}

			/* checking if this line is really "user_color" */
			substringFromString(tempStr, 2, 11, stringToRead);


			if (strcmp(tempStr, "user_color") == 0){

				/* stringToRead[12] can either be 'w' (for white) or b (for black) */
				if (stringToRead[13] == 'W'){
					user_color = WHITE;
					*current_player_color = WHITE;
				}

				else{
					user_color = BLACK;
					*current_player_color = BLACK;
				}
			}
			else{
				/* default value */
				user_color = WHITE;
				*current_player_color = WHITE;
				fseek(filePointer, -1, SEEK_CUR);
			}
		}
	}
	else{
		/* default value */
		game_mode = 1;
		fseek(filePointer, -1, SEEK_CUR);
	}

	/* loading color of current player */
	if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
		fclose(filePointer);
		return 0;
	}

	/* checking if this line is really "next_turn" */
	substringFromString(tempStr, 2, 10, stringToRead);

	if (strcmp(tempStr, "next_turn") == 0){

		/* stringToRead[11] can either be 'W' (for white) or B (for black) */
		if (stringToRead[12] == 'W')
			*current_player_color = WHITE;
		else
			*current_player_color = BLACK;
	}
	else {
		/* in AI, we assume that the current player color is equal to the user_color */
		if (game_mode == 1){
			/* default value */
			*current_player_color = WHITE;
			fseek(filePointer, -1, SEEK_CUR);
		}
		fseek(filePointer, -1, SEEK_CUR);
	}


	/* loading the board */

	/* not interesting line   (<board>) */
	if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
		fclose(filePointer);
		return 0;
	}


	for (i = 0; i < BOARD_SIZE; i++){
		/* loading the i-th row */
		if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
			fclose(filePointer);
			return 0;
		}

		for (j = 0; j < BOARD_SIZE; j++){
			if (stringToRead[7 + j + 2] == '_') /* +2 for the tabs */
				board[j][BOARD_SIZE - i - 1] = EMPTY;
			else
				board[j][BOARD_SIZE - i - 1] = stringToRead[7 + j + 2];
		}
	}

	/*  (</board>) */
	if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
		fclose(filePointer);
		return 0;
	}

	/*  (</game>) added by me */
	if (fgets(stringToRead, STRING_SIZE - 1, filePointer) == NULL){
		fclose(filePointer);
		return 0;
	}

	fclose(filePointer);

	return 1;
}




