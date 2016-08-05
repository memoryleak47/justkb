#include "justkb.hpp"

#include <X11/extensions/XTest.h>
#include <string>
#include <iostream>

Display* Jkb::display;
bool Jkb::running;

void Jkb::start()
{
	init();
	run();
	uninit();
}

void Jkb::init()
{
	running = true;
	if (NULL == (display = XOpenDisplay(NULL)))
	{
		std::cout << "display = XopenDisplay(NULL)" << std::endl;
		running = false;
	}

	XGrabKeyboard(display, DefaultRootWindow(display), true, GrabModeAsync, GrabModeAsync, CurrentTime);
}

void Jkb::uninit()
{
	XUngrabKeyboard(display, CurrentTime);

	if (XCloseDisplay(display))
	{
		std::cout << "XCloseDisplayer(display)" << std::endl;
		running = false;
	}
}

void Jkb::handleEvent(const XEvent &x_event)
{
	switch (x_event.type)
	{
		case KeyPress:
		{
			int keycode = ((XKeyPressedEvent*)&x_event)->keycode;
			if (keycode == 24) // q
			{
				std::cout << "q is pressed -> quitting" << std::endl;
				running = false;
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

void Jkb::run()
{
	XEvent ev;
	unsigned int counter = 0;

	while (running)
	{
		XNextEvent(display, &ev);
		handleEvent(ev);
		if (counter > 100)
		{
			std::cout << "counter > 100 -> quitting" << std::endl;
			running = false;
		}
		counter++;
	}
}

void Jkb::sendKey(int keycode, bool pressed)
{
	XUngrabKeyboard(display, CurrentTime);
	XTestFakeKeyEvent(display, keycode, pressed, 0);
	XGrabKeyboard(display, DefaultRootWindow(display), true, GrabModeAsync, GrabModeAsync, CurrentTime);
}
