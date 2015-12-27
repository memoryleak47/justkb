#include "uinterface.h"

int main()
{
	init();
	sleep(1);
	applyKeyEvent(KEY_A, 1);
	applyKeyEvent(KEY_A, 0);
	sleep(1);
	uninit();
	return 0;
}
