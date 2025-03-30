#include "display.h"
#include "hop.h"
void store(char input[MAX_INPUT_LENGTH]);
void add(char **log, char command[MAX_INPUT_LENGTH]);
void print();
void addtofile(char **log);
void loadcommands(char **log);
void clear();
void execute_c(int cno);