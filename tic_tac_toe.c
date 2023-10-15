//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/shm.h>
#include <stdbool.h>

#define IPC_RESULT_ERROR (-1)

int printBoard(char * gameBoard, int n);
int checkWin(char* gameBoard, int n);
int get_shared_memory(char* filename, int size);
char* attach_shared_memory(char* filename, int size);
bool detach_shared_memory(char *block);
bool destroy_shared_memory(char *filename);
char* populateBoard(int n);

int main (int argc, char* argv[])
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

	//If we've made it here, then there is only 2 arguments
	//create an NxN board, where N is the given argument
	int n = atoi(argv[1]);
	int boardSize = n * n;

	printf("Welcome to Tic-Tac-Toe!\n");
	printf("The goal is to get %d in a row!\n", n);
	printf("Player 1 uses x's\n");
	printf("Player 2 uses o's\n");

	//create a shared array for the gameBoard, for both processes to use
	char * gameBoard = attach_shared_memory("tic_tac_toe.c", boardSize);
	if(gameBoard == NULL)
	{
		printf("ERROR: Couldn't create shared memory\n");
		return -1;
	}

	gameBoard = populateBoard(n);
}

char* populateBoard(int n)
{
	char* gameBoard = malloc(n * n);

	for(int i = 0; i < (n * n); i += n)
	{
		gameBoard[i] = '-';
	}

	return gameBoard;
}

int printBoard(char* gameBoard, int n)
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

//will iterate over entire array, checking for any wins, return 1 if X's (player1) wins, and 2 if O's (player2) wins, or 0 for no winner
int checkWin(char* gameBoard, int n)
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

	return 0;
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
	return shmget(key, size, IPC_CREAT);

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

	return (shmctl(shared_memory_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}
