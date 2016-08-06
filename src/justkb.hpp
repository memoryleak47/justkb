#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <X11/Xlib.h>

class Jkb
{
	public:
		static void start();
		static void init();
		static void uninit();
		static void run();
	private:
		static void handleEvent(const XEvent &x_event);

		// inject
		static void sendKey(int keycode, bool pressed);
		static void sendKeys(int keycodes[], bool pressed[], unsigned int number);
		static void sendKey(KeySym keysym, bool pressed);
		static void sendKeys(KeySym keysyms[], bool pressed[], unsigned int number);
		static void type(int keycodes[], unsigned int number);
		static void type(KeySym keysyms[], unsigned int number);

		static void grab();
		static void ungrab();

		static Display* display;
		static bool running;

};
