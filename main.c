/*Main includes*/
#include <stdio.h>
#include <MLV/MLV_all.h>

/*Include own headers*/
#include "headers/structs.h"
#include "headers/functions.h"

int main(int argc, char *argv[])
{
	gameObj = initGame();
	mainMenu();

	quitGame();
	
	return 0;
}