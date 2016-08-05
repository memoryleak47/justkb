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
		static void sendKey(int keycode, bool pressed);
		static Display* display;
		static bool running;
};
