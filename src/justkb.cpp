#include "justkb.hpp"

#include <string.h>
#include <iostream>
#include <linux/input.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <X11/keysym.h>

constexpr int RELEASE = 0;
constexpr int PRESS = 1;
constexpr int MAINKEYCODE = 82;

bool Jkb::running;
int Jkb::grab_fd; // event fd
int Jkb::uinput_fd; // uinput fd
struct input_event Jkb::ev;
Display* Jkb::display;

void Jkb::start()
{
	init();
	run();
	uninit();
}

void Jkb::init()
{
	running = true;

	// initing uinput
	assert((uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) >= 0); // may also be in /dev/input/uinput
	assert(ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY) >= 0);

	for (int i = 0; i < 256; i++)
	{
		assert(ioctl(uinput_fd, UI_SET_KEYBIT, i) >= 0);
	}

	uinput_user_dev udev;
	memset(&udev, 0, sizeof(udev));

	snprintf(udev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	udev.id.bustype = BUS_USB;
	udev.id.vendor  = 0x1234;
	udev.id.product = 0xfedc;
	udev.id.version = 1;

	assert(write(uinput_fd, &udev, sizeof(udev)) >= 0);
	assert(ioctl(uinput_fd, UI_DEV_CREATE) >= 0);

	assert((grab_fd = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY)) >= 0);
	assert(ioctl(grab_fd, EVIOCGRAB, 1) >= 0);

	// init x
	assert(display = XOpenDisplay(NULL));
}

void Jkb::uninit()
{
	assert(ioctl(grab_fd, EVIOCGRAB, 0) >= 0);
	assert(close(grab_fd) >= 0);

	assert(ioctl(uinput_fd, UI_DEV_DESTROY) >= 0);
	assert(close(uinput_fd) >= 0);

	// uninit x
	assert(0 == XCloseDisplay(display));
}

void Jkb::handleKeyEvent(int keycode, int value)
{
	if (keycode == 16)
	{
		printf("q -> exit\n");
		running = false;
	}
	else if (value == PRESS)
	{
		sendKeysym(XK_J);
	}
}

void Jkb::run()
{
	unsigned int counter = 0;
	ssize_t n;
	struct input_event ev;

	while (true)
	{
		if (counter++ > 100)
		{
			printf("counter > 100 -> exit\n");
			running = false;
		}

		if (not running)
		{
			printf("not running\n");
			return;
		}


		n = read(grab_fd, &ev, sizeof ev);
		if (n == (ssize_t)-1)
		{
			if (errno == EINTR)
				continue;
			else
				break;
		}
		else
		{
			if (n != sizeof ev)
			{
				errno = EIO;
				break;
			}
		}

		if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
		{
			handleKeyEvent(ev.code, ev.value);
		}
	}
}

// seems like addKeyEvent(int, int) insertions are not grabbed :D
void Jkb::addKeyEvent(int keycode, int value)
{
	// inject
	memset(&ev, 0, sizeof(ev));
	ev.type = EV_KEY;
	ev.code = keycode;
	ev.value = value;
	assert(write(uinput_fd, &ev, sizeof(ev)) >= 0);
}

void Jkb::sendKeysym(KeySym keysym)
{
	// MAINKEYCODE => keysym
	KeySym k[]{keysym, keysym, keysym, keysym, keysym, keysym, keysym, keysym};
	assert(0 == XChangeKeyboardMapping(display, MAINKEYCODE, 8, k, 1));

	// imitate MAINKEYCODE
	addKeyEvent(MAINKEYCODE, RELEASE);
	addKeyEvent(MAINKEYCODE, PRESS);
	addKeyEvent(MAINKEYCODE, RELEASE);
	flush(); // XXX XSync() OR XFlush() don't work
}

void Jkb::flush()
{
	// sync
	memset(&ev, 0, sizeof(ev));
	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;
	assert(write(uinput_fd, &ev, sizeof(ev)) >= 0);
}
