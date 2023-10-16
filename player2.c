//Marshall Anderson
//CS 470: Operating Systems
//Lab 2: Tic-Tac-Toe

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <spawn.h>

#define IPC_RESULT_ERROR (-1)
//filename is player 1
#define FILENAME "tic_tac_toe.c"

//functions
char* turn(char* gameBoard);
int checkWin(char* gameBoard);
int printBoard(char * gameBoard);
int get_shared_memory(char* filename, int size);
char* attach_shared_memory(char* filename, int size);
bool detach_shared_memory(char *block);
bool destroy_shared_memory(char *filename);

//global variables
int n;
bool isWinner;

int main(int argc, char* argv[])
{
	//Check for too many inputs
	if(argc > 2)
	{
		printf("Error, too many arguments supplied.\n");
		exit(0);
	}
	//Check for too few inputs
	else if(argc == 1)
	{
		printf("Error, too few arguments supplied.\n");
		exit(0);
	}
	//ERROR DETECTION: CHECK FOR WRONG INPUT IN ARGV[1]

	n = atoi(argv[1]);
	int boardSize = n * n;
	//pull the gameBoard from player1, it's in shared memory
	char* gameBoard = attach_shared_memory(FILENAME, boardSize);
	

	while(!isWinner)
	{
		//take the turn and return the updated gameBoard
		turn(gameBoard);
	}
}


char* turn(char* gameBoard)
{

	int winValue = 0;
	//print the board, then check for a winner
	printBoard(gameBoard);
	winValue += checkWin(gameBoard);
	if(winValue == 1)
	{
		//output to player1 that they won, and output to player2 that they lost
		printf("Player 2 loses!\n");
		isWinner = true;
		return gameBoard;
	}
	else if(winValue == 2)
	{
		//output to player 2 that they won, and output to player 1 that they lost
		printf("Player 2 wins!\n");
		isWinner = true;
		return gameBoard;
	}
	else if(winValue == -1)
	{
		printf("The game is a draw!\n");
		isWinner = true;
		return gameBoard;
	}

	//get the row and column value to place the piece at
	printf("It is your turn! Please enter the row you wish to place a piece at.\n");
	int row;

	scanf("%i", &row);

	printf("Please enter the column you wish to place a piece at.\n");
	int column;

	scanf("%i", &column);

	//check if input is out of bounds
	if(column > n || row > n)
	{
		printf("Invalid move, this goes beyond the boundaires of the board, try again.\n");
	}
	//check if piece is already there
	else if(gameBoard[(row-1) + (n * (column - 1))] == 'x')
	{
		printf("Invalid move, there is already a piece there.\n");
	}
	else if(gameBoard[(row-1) + (n * (column - 1))] == 'o')
	{
		printf("Invalid move, there is already a piece there.\n");
	}
	else
	{
		//add the users input to the board, since it is a 1d array, the location is x + n * y.
		gameBoard[(row-1) + (n * (column - 1))] = 'x';
		
	}

	return gameBoard;
}

int printBoard(char* gameBoard)
{
	int boardSize = n * n;
	//print the gameboard
	for(int i = 0; i < boardSize; i += n)
	{
		for(int j = 0; j < n; j++)
		{
			//if its the first of a row
			if(j ==  0) 
				{
					printf("| ");
				}
			//print the value followed by a separator
			printf("%c", gameBoard[i + j]);
			printf(" | ");

			//if were at the end of the row, print a new line
			if(j == n - 1) 
			{
				printf("\n");
			}
		}
	}
}

//will iterate over entire array, checking for any wins, return 1 if X's (player1) wins, and 2 if O's (player2) wins, or 0 for no winner, -1 for draw
int checkWin(char* gameBoard)
{
	int boardSize = n * n;
	
	//check horizontal
	for(int i = 0; i < boardSize; i += n)
	{
		int win1 = 0;
		int win2 = 0;
		for(int j = 0; j < n; j++)
		{
			if(gameBoard[i + j] == 'x') win1++;
			else if(gameBoard[i + j] == 'o') win2++;
		}

		if(win1 == n) return 1;
		else if(win2 == n) return 2;
	}

	//check vertical
	for(int i = 0; i < n; i += 1)
	{
		int win1 = 0;
		int win2 = 0;
		for(int j = 0; j < boardSize; j += n)
		{
			if(gameBoard[i + j] == 'x') win1++;
			else if(gameBoard[i + j] == 'o') win2++;
		}

		if(win1 == n) return 1;
		else if(win2 == n) return 2;
	}

	//check diagonal
	//if x + y = n + 1, it is a diagonal
	//downward diagonal, equal to i * height + i
	int upDiagX = 0;
	for(int i = 0; i < n; i++)
	{
		if(gameBoard[i * n + i] == 'x') upDiagX++;
	}
	if(upDiagX == n) return 1;

	int upDiagO = 0;
	for(int i = 0; i < n; i++)
	{
		if(gameBoard[i * n + i] == 'o') upDiagO++;
	}
	if(upDiagO == n) return 2;


	//upward diagonal
	//start at boardSize - n, and increment down by i % (n-1) == 2 is true
	
	//check for draws
	//if all the other checks haven't returned anything, and the board is full
	//then its a draw
	int filledIn = 0;
	for(int i = 0; i < boardSize; i++)
	{
		if(gameBoard[i] == 'x' || gameBoard[i] == 'o') filledIn++;
	}
	if(filledIn == boardSize)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int get_shared_memory(char* filename, int size)
{
	key_t key;

	//use ftok to get a key for the shared memory
	key = ftok(filename, 0);
	if(key == IPC_RESULT_ERROR)
	{
		return IPC_RESULT_ERROR;
	}
	
	//get the shared memory, or create it if it doesn't exist
	return shmget(key, size, 0644 | IPC_CREAT);

}

char* attach_shared_memory(char* filename, int size)
{
	//call get_shared_memory
	int shared_memory_id = get_shared_memory(filename, size);
	char *result;

	//error check
	if(shared_memory_id == IPC_RESULT_ERROR)
	{
		return NULL;
	}

	//map the shared memory into this process's memory
	//and get a pointer to it
	result = shmat(shared_memory_id, NULL, 0);
	if(result == (char *)IPC_RESULT_ERROR)
	{
		return NULL;
	}

	//return the pointer
	return result;
}

bool detach_shared_memory(char *memory)
{
	return ((shmdt(memory) != IPC_RESULT_ERROR));
}

bool destroy_shared_memory(char *filename)
{
	int shared_memory_id = get_shared_memory(filename, 0);

	if(shared_memory_id == IPC_RESULT_ERROR)
	{
		return NULL;
	}

	return (shmctl(shared_memory_id, 0644 | IPC_RMID, NULL) != IPC_RESULT_ERROR);
}
