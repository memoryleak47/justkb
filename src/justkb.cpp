#include "justkb.h"

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

XKeyEvent createKeyEvent(bool press, int keycode, int modifiers)
{
	Window root = XDefaultRootWindow(x_display);
	Window focus;
	int revert;
	XGetInputFocus(x_display, &focus, &revert);

	XKeyEvent event;

	event.display = x_display;
	event.window = focus;
	event.root = root;
	event.subwindow	= None;
	event.time = CurrentTime;
	event.x = 1;
	event.y = 1;
	event.x_root = 1;
	event.y_root = 1;
	event.same_screen = true;
	event.keycode = keycode;
	event.state = modifiers;

	if (press)
		event.type = KeyPress;
	else
		event.type = KeyRelease;

	return event;
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
			XKeyEvent event = createKeyEvent(true, keycode+1 /* TODO remove +1 */, 0);
			XSendEvent(event.display, event.window, true, KeyPressMask, (XEvent*)&event);
			break;
		}
		case KeyRelease:
		{
			int keycode = ((XKeyPressedEvent*)&x_event)->keycode;
			XKeyEvent event = createKeyEvent(false, keycode+1 /* TODO remove +1 */, 0);
			XSendEvent(event.display, event.window, true, KeyPressMask, (XEvent*)&event);
			break;
		}
		case ButtonPress:
		case ButtonRelease:
			printf("TODO");
			break;
		case Expose:
			printf("whats going on here in expose?");
			break;
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
		if (counter > 10)
		{
			quit("counter > 10 -> quitting");
		}
		counter++;
	}
}
