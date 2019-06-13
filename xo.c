#include <stdio.h>
#define STARTING_PLAYER PLAYER
#define BOARD_SIZE 3
#define CONSEC_TO_WIN 3
#define MAX_REC_DEPTH 7

typedef struct Cursor {
	int row;
	int col;
} Cursor_t;

typedef enum Player { PC = -1, EMPTY = 0, PLAYER = 1 } Player_t;
typedef enum GameMode { ACTIVE, WIN, TIE } GameMode_t;

int moves = 0;

char enumToXO(Player_t player) {
	if (player == STARTING_PLAYER) {
		return 'X';
	}
	if (player == -STARTING_PLAYER) {
		return 'O';
	}
	return ' ';
}

void printGame(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE], Cursor_t cursor) {
	printf("\n\n\n\n\n\n\n\n");
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (cursor.row == i && cursor.col == j) {
				printf("|%c|", enumToXO(curBoardState[i][j]));
			}
			else {
				printf(":%c:", enumToXO(curBoardState[i][j]));
			}
		}
		putchar('\n');
	}
}

GameMode_t checkSquareMode(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE], Cursor_t UpLeftCorner) {
	int sumRaw[CONSEC_TO_WIN] = { 0 }, sumCol[CONSEC_TO_WIN] = { 0 }, sumDiag[2] = { 0 };

	// sum up diagonals of the inner squere
	for (int idx = 0; idx < CONSEC_TO_WIN; ++idx) {
		sumDiag[0] += curBoardState[idx + UpLeftCorner.row][idx + UpLeftCorner.col];
		sumDiag[1] += curBoardState[idx + UpLeftCorner.row][CONSEC_TO_WIN - idx - 1 + UpLeftCorner.col];
	}

	// check if diagonals win
	if (sumDiag[0] == CONSEC_TO_WIN || sumDiag[0] == -CONSEC_TO_WIN || sumDiag[1] == CONSEC_TO_WIN || sumDiag[1] == -CONSEC_TO_WIN) {
		return WIN;
	}

	// sum up raws and columns
	for (int i = 0; i < CONSEC_TO_WIN; ++i) { //TODO
		for (int j = 0; j < CONSEC_TO_WIN; ++j) {
			sumCol[j] += curBoardState[i + UpLeftCorner.row][j + UpLeftCorner.col];
			sumRaw[i] += curBoardState[i + UpLeftCorner.row][j + UpLeftCorner.col];
		}
	}

	// check if columns win
	for (int idx = 0; idx < CONSEC_TO_WIN; ++idx) {
		if (sumRaw[idx] == CONSEC_TO_WIN || sumRaw[idx] == -CONSEC_TO_WIN || sumCol[idx] == CONSEC_TO_WIN || sumCol[idx] == -CONSEC_TO_WIN) {
			return WIN;
		}
	}

	return ACTIVE;
}

int isBoardFull(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (curBoardState[i][j] == EMPTY) {
				return 0;
			}
		}
	}
	return 1;
}

GameMode_t checkGameMode(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i <= BOARD_SIZE - CONSEC_TO_WIN; ++i) {
		for (int j = 0; j <= BOARD_SIZE - CONSEC_TO_WIN; ++j) {
			Cursor_t UpLeftCornerOfSquare = { i, j };
			GameMode_t gameMode = checkSquareMode(&curBoardState[0], UpLeftCornerOfSquare);
			if (gameMode == WIN) {
				return gameMode;
			}
		}
	}

	if (isBoardFull(&curBoardState[0])) {
		return TIE;
	}
	return ACTIVE;
}

int isStateSafe(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE], Player_t curPlayer, int depth) {
	GameMode_t gameMode = checkGameMode(&curBoardState[0]);

	// the game was won by the previous player
	if (gameMode == WIN) {
		return (-curPlayer == PC) ? 1 : 0;
	}

	// tie is a safe result
	if (gameMode == TIE) {
		return 1;
	}

	// we are deep enough in the recursion and no danger was found
	// so we are safe for now..
	if (depth > MAX_REC_DEPTH) {
		return 1;
	}

	// if we got here, there has to be at least one empty spot
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			// find an empty spot and place a mark there
			if (curBoardState[i][j] == EMPTY) {
				curBoardState[i][j] = curPlayer;
			}
			else {
				continue;
			}
			// if the PC is playing one safe place is good enough
			if (curPlayer == PC && isStateSafe(&curBoardState[0], -curPlayer, depth + 1)) {
				curBoardState[i][j] = 0; //delete the move, this function only checks for safty
				return 1;
				// if the PLAYER is playing, even one unsafe option is bad!
				// we assume a good player will choose exactly this option and thus possibly win
			}
			else if (curPlayer == PLAYER && !isStateSafe(&curBoardState[0], -curPlayer, depth + 1)) {
				curBoardState[i][j] = 0; //delete the move, this function only checks for safty
				return 0;
			}
			curBoardState[i][j] = 0; //delete the move and continue checking the ather spots
		}
	}

	return curPlayer == PLAYER;
}

int isIdxValid(int rawIdx, int culIdx) {
	int isRawValid = rawIdx < BOARD_SIZE && rawIdx >= 0;
	int isCulValid = culIdx < BOARD_SIZE && culIdx >= 0;

	return isRawValid && isCulValid;
}

void getInput(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE]) {
	int rawIdx = 0;
	int culIdx = 0;
	while (1) {
		char key = 0;
		size_t len = 0;
		char line[2] = { 0 };
		static int keyToIdxDeltaMap[9][2] = { {1, -1}, {1, 0}, {1, 1}, {0, -1}, {0, 0}, {0, 1}, {-1, -1}, {-1, 0}, {-1, 1} };
		key = getchar();
		getchar();
		if (isIdxValid(rawIdx + keyToIdxDeltaMap[key - '1'][0], culIdx + keyToIdxDeltaMap[key - '1'][1])) {
			rawIdx += keyToIdxDeltaMap[key - '1'][0];
			culIdx += keyToIdxDeltaMap[key - '1'][1];
		}
		if (key == '5') {
			if (curBoardState[rawIdx][culIdx] == EMPTY) {
				curBoardState[rawIdx][culIdx] = PLAYER;
				break;
			}
			else {
				puts("This Move aint legal!");
			}
		}
		else if (key >= '0' && key <= '9') {
			Cursor_t cursor = { rawIdx, culIdx };
			printGame(&curBoardState[0], cursor);
		}
	}
}

void executeSafeMove(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			// find an empty spot and place a mark there
			if (curBoardState[i][j] == EMPTY) {
				Cursor_t cursor = { i, j };
				printGame(&curBoardState[0], cursor);
				curBoardState[i][j] = PC;
				if (isStateSafe(&curBoardState[0], PLAYER, 1)) {
					return; // keep this move
				}
				else {
					curBoardState[i][j] = EMPTY; //get back to original state
				}
			}
		}
	}
	puts("ERROR: no safe move!");
}

void runGame(Player_t curBoardState[BOARD_SIZE][BOARD_SIZE], Player_t curPlayer) {
	moves++; // DEBUG
	if (curPlayer == PLAYER) {
		getInput(&curBoardState[0]);
	}
	else {
		executeSafeMove(&curBoardState[0]);
	}

	Cursor_t cursor = { 0, 0 };
	printGame(&curBoardState[0], cursor);

	Cursor_t UpLeftCornerOfSquare = { 0, 0 };
	GameMode_t gameMode = checkGameMode(&curBoardState[0]);
	if (gameMode != ACTIVE) {
		puts("GAME OVER!");
		return;
	}

	runGame(&curBoardState[0], -curPlayer);
}

int main()
{
	Player_t curBoardState[BOARD_SIZE][BOARD_SIZE] = { EMPTY };
	Player_t startingPlayer = STARTING_PLAYER;
	Cursor_t cursor = { 0, 0 };
	if (BOARD_SIZE < CONSEC_TO_WIN) {
		puts("BOARD_SIZE should be bigger then CONSEC_TO_WIN..");
		return 1;
	}
	printGame(&curBoardState[0], cursor);
	runGame(&curBoardState[0], startingPlayer);

	return 0;
}