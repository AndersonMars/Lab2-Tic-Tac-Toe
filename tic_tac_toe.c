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
#include <ctype.h>

//include memory.h for the shared memory functions
#include "memory.c"

#define IPC_RESULT_ERROR (-1)
#define FILENAME "tic_tac_toe.c"

//environ for spawn call
extern char **environ;

//global variable
int n;
bool isWinner;
int winValue;

int main (int argc, char* argv[])
{
	//Check for too many inputs
	if(argc > 2)
	{
		printf("Error, too many arguments supplied.\n");
		exit(0);
	}
	//Check for too few inputs
	else if(argc < 2)
	{
		printf("Error, too few arguments supplied.\n");
		exit(0);
	}
	//ERROR DETECTION: CHECK FOR WRONG INPUT IN ARGV[1]
	if(isdigit(atoi(argv[1])))
	{
		printf("Error: please only input the board size as the second argument.\n");
		exit(0);
	}

	//If we've made it here, then there is only 2 arguments
	//create an NxN board, where N is the given argument
	n = atoi(argv[1]);
	int boardSize = n * n;
	char* childArguments[2] = {"./player2", argv[1]};
	
	//boolean to track if a winner has been found
	isWinner = false;
	

	printf("Welcome to Tic-Tac-Toe!\n");
	printf("The goal is to get %d in a row!\n", n);
	printf("Player 1 uses x's\n");
	printf("Player 2 uses o's\n");

	//create a shared array for the gameBoard, for both processes to use
	char * gameBoard = attach_shared_memory(FILENAME, boardSize);
	
	if(gameBoard == NULL)
	{
		printf("Error: Couldn't create shared memory\n");
		return -1;
	}
	
	//populate the board
	gameBoard = populateBoard(gameBoard, n);
	
	//create a separate process that will represent player 2
	int status;
	int child_id;

	while(!isWinner)
	{
		//before x's turn, check for a win
		winValue = 0;

		winValue += checkWin(gameBoard, n);
		if(winValue == 1)
		{
			//output to player1 that they won, and output to player2 that they lost
			printf("Player 1 wins!\n");
			detach_shared_memory(gameBoard);
			isWinner = true;
			break;
		}
		else if(winValue == 2)
		{
			//output to player 2 that they won, and output to player 1 that they lost
			printf("Player 1 loses!\n");
			detach_shared_memory(gameBoard);
			isWinner = true;
			break;
		}
		else if(winValue == -1)
		{	
			printf("The game is a draw!\n");
			detach_shared_memory(gameBoard);
			isWinner = true;
			break;
		}


		printBoard(gameBoard, n);

		//Take the turn and return the updated gameBoard
		turn(gameBoard, 'x', n);

		//after x's turn, check for a winner
		winValue = 0;

		winValue += checkWin(gameBoard, n);
		if(winValue == 1)
		{
			printBoard(gameBoard, n);
			//output to player1 that they won, and output to player2 that they lost
			printf("Player 1 wins!\n");
			detach_shared_memory(gameBoard);
			isWinner = true;
		}
		else if(winValue == 2)
		{
			printBoard(gameBoard, n);
			//output to player 2 that they won, and output to player 1 that they lost
			printf("Player 1 loses!\n");
			detach_shared_memory(gameBoard);
			isWinner = true;
		}
		else if(winValue == -1)
		{	
			printBoard(gameBoard, n);
			printf("The game is a draw!\n");
			detach_shared_memory(gameBoard);
			isWinner = true;
		}

		//after x's turn, call o.
		if(0 != posix_spawn(&child_id, childArguments[0], NULL, NULL, childArguments, environ))
		{
			perror("spawn failed");
			exit(1);
		}

		wait(&child_id);
	}

	destroy_shared_memory(gameBoard);
}