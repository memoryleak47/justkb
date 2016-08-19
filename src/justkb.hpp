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
		static void addKeyEvent(int keycode, int value);
		static void sendKey(int keycode);
		static void sendKeys(int* keycodes, int count);
		static void flush();

		static bool running;
		static int uinput_fd;
		static int grab_fd;
		static struct input_event ev;
};
