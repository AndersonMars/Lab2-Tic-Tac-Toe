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

//include memory.h for the shared memory functions
#include "memory.c"

//filename is player 1
#define FILENAME "tic_tac_toe.c"

//global variables
int n;

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
	

	//check for the win before o's turn
	int winValue = 0;

	winValue += checkWin(gameBoard, n);
	if(winValue == 1)
	{
		//output to player1 that they won, and output to player2 that they lost
		printf("Player 2 loses!\n");
		detach_shared_memory(gameBoard);
		return 0;
	}
	else if(winValue == 2)
	{
		//output to player 2 that they won, and output to player 1 that they lost
		printf("Player 2 wins!\n");
		detach_shared_memory(gameBoard);
		return 0;
	}
	else if(winValue == -1)
	{	
		printf("The game is a draw!\n");
		detach_shared_memory(gameBoard);
		return 0;
	}

	printBoard(gameBoard, n);

	//take the turn and return the updated gameBoard
	turn(gameBoard, 'o', n);

	//check for the win after o's turn
	winValue = 0;

	winValue += checkWin(gameBoard, n);
	if(winValue == 1)
	{
		//output to player1 that they won, and output to player2 that they lost
		printf("Player 2 loses!\n");
		detach_shared_memory(gameBoard);
		return 0;
	}
	else if(winValue == 2)
	{
		printBoard(gameBoard, n);
		//output to player 2 that they won, and output to player 1 that they lost
		printf("Player 2 wins!\n");
		detach_shared_memory(gameBoard);
		return 0;
	}
	else if(winValue == -1)
	{	
		printBoard(gameBoard, n);
		printf("The game is a draw!\n");
		detach_shared_memory(gameBoard);
		return 0;
	}
}