#include "memory.h"

char* turn(char* gameBoard, char piece, int n)
{

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
		if(piece == 'o') gameBoard[(row-1) + (n * (column - 1))] = 'o';
		else gameBoard[(row-1) + (n * (column - 1))] = 'x';
	}

	return gameBoard;
}

char* populateBoard(char* gameBoard, int n)
{
	int size = n * n;
	for(int i = 0; i < size; i += n)
	{
		for(int j = 0; j < n; j++)
		{
			gameBoard[i + j] = '-';
		}
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

	return 0;
}

//will iterate over entire array, checking for any wins, return 1 if X's (player1) wins, and 2 if O's (player2) wins, or 0 for no winner, -1 for draw
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

	//check diagonal
	//if x + y = n + 1, it is a diagonal
	//downward diagonal, equal to i * height + i
	int downDiagX = 0;
	for(int i = 0; i < n; i++)
	{
		if(gameBoard[i * n + i] == 'x') downDiagX++;
	}
	if(downDiagX == n) return 1;

	int downDiagO = 0;
	for(int i = 0; i < n; i++)
	{
		if(gameBoard[i * n + i] == 'o') downDiagO++;
	}
	if(downDiagO == n) return 2;

	//upward diagonal
	//start at n-1 icrement by n-1, this will move from top right to bottom left
	int upDiagX = 0;
	for(int i = n-1; i < boardSize; i+= (n-1))
	{
		//make sure it isn't the first or last
		if(gameBoard[i] == 'x') upDiagX++;
	}
	if(upDiagX == n) return 1;

	int upDiagO = 0;
	for(int i = n-1; i < boardSize; i+= (n-1))
	{
		//if part of the diagonal
		if(gameBoard[i] == 'o') upDiagO++;
	}
	if(upDiagO == n) return 2;

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
	if(key == -1)
	{
		return -1;
	}
	
	//get the shared memory, or create it if it doesn't exist
	return shmget(key, size, 0666 | IPC_CREAT);

}

char* attach_shared_memory(char* filename, int size)
{
	//call get_shared_memory
	int shared_memory_id = get_shared_memory(filename, size);
	char *result;

	//error check
	if(shared_memory_id == -1)
	{
		return NULL;
	}

	//map the shared memory into this process's memory
	//and get a pointer to it
	result = shmat(shared_memory_id, NULL, 0);
	if(result == (char *)-1)
	{
		return NULL;
	}

	//return the pointer
	return result;
}

bool detach_shared_memory(char *memory)
{
	return ((shmdt(memory) != -1));
}

bool destroy_shared_memory(char *filename)
{
	int shared_memory_id = get_shared_memory(filename, 0);

	if(shared_memory_id == -1)
	{
		return NULL;
	}

	return (shmctl(shared_memory_id, 0666 | IPC_RMID, NULL) != -1);
}
