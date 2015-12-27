#include "uinterface.h"

int main()
{
	init();
	moveMouse(-2000, -2000);
	moveMouse(400, 300);
	applyKeyEvent(KEY_A, 1);
	applyKeyEvent(KEY_A, 0);
	uninit();
	return 0;
}
