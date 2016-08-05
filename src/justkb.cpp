#include "justkb.h"

#include <X11/extensions/XTest.h>

#define MAX_COUNTER 30

#define quit(str) do { \
	printf("\n%s\n", str); \
	quit = true; \
	} while (0)

bool quit = false;

int ui_fd;
Display *x_display;

void init()
{
	if (NULL == (x_display = XOpenDisplay(NULL)))
	{
		die("x_display = XopenDisplay(NULL)");
	}

	XGrabKeyboard(x_display, DefaultRootWindow(x_display), true, GrabModeAsync, GrabModeAsync, CurrentTime);
}

void uninit()
{
	XUngrabKeyboard(x_display, CurrentTime);

	if (XCloseDisplay(x_display))
	{
		die("XCloseDisplayer(x_display)");
	}
}

void handleEvent(const XEvent &x_event)
{
	switch (x_event.type)
	{
		case KeyPress:
		{
			int keycode = ((XKeyPressedEvent*)&x_event)->keycode;
			if (keycode == 24) // q
			{
				quit("q is pressed -> quitting");
			}
			sendKey(42, true);
			break;
		}
		case KeyRelease:
		{
			sendKey(42, false);
			break;
		}
		case ButtonPress:
		case ButtonRelease:
		case Expose:
		case MotionNotify:
		case ConfigureNotify:
		default:
			break;
	}
}

void run()
{
	XEvent x_event;
	unsigned int counter = 0;

	while (!quit)
	{
		XNextEvent(x_display, &x_event);
		handleEvent(x_event);
		if (counter > MAX_COUNTER)
		{
			quit("counter > MAX_COUNTER -> quitting");
		}
		counter++;
	}
}

void sendKey(int keycode, bool pressed)
{
	XUngrabKeyboard(x_display, CurrentTime);
	XTestFakeKeyEvent(x_display, keycode, pressed, 0);
	XGrabKeyboard(x_display, DefaultRootWindow(x_display), true, GrabModeAsync, GrabModeAsync, CurrentTime);
}
