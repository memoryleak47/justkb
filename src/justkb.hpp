#include <linux/uinput.h>

class Jkb
{
	public:
		static void start();
		static void init();
		static void uninit();
		static void run();
	private:
		static void handleKeyEvent(int keycode, int value);

		// inject
		static void sendKey(int keycode, int value);

		static void grab();
		static void ungrab();

		static bool running;
		static int ufd;
		static int efd;
};
