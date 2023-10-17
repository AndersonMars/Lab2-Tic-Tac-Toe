#ifndef MEMORY_H_ //start of header
#define MEMORY_H_

char* turn(char* gameBoard, char piece, int n);
char* populateBoard(char* gameBoard, int n);
int printBoard(char* gameBoard, int n);
int checkWin(char* gameBoard, int n);
int get_shared_memory(char* filename, int size);
char* attach_shared_memory(char* filename, int size);
bool detach_shared_memory(char* memory);
bool destroy_shared_memory(char* filename);

#endif //end of header