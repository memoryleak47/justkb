#include "uinterface.h"

int main()
{
	init();
	applyKeyEvent(KEY_A, 1);
	applyKeyEvent(KEY_A, 0);
	uninit();
	return 0;
}
