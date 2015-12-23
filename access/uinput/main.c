#include "uinterface.h"

extern void init();
extern void mouseMove(int dx, int dy);
extern void uninit();

int main()
{
	init();
	moveMouse(30, 30);
	uninit();
	return 0;
}
