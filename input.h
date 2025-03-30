#include "display.h"

#include "hop.h"
#define maxcommandlength 4096

char *trimspace(char *str);

void execute_command(char *argv[maxcommandlength],int i);       
void execute(char input[MAX_INPUT_LENGTH]);