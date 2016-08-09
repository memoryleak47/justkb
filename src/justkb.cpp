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

bool Jkb::running;
int Jkb::efd; // event fd
int Jkb::ufd; // uinput fd

void Jkb::start()
{
	init();
	run();
	uninit();
}

void Jkb::init()
{
	running = true;
	assert((ufd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) >= 0); // may also be in /dev/input/uinput
	assert(ioctl(ufd, UI_SET_EVBIT, EV_KEY) >= 0);

	for (int i = 0; i < 256; i++)
	{
		assert(ioctl(ufd, UI_SET_KEYBIT, i) >= 0);
	}

	uinput_user_dev udev;
	memset(&udev, 0, sizeof(udev));

	snprintf(udev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	udev.id.bustype = BUS_USB;
	udev.id.vendor  = 0x1234;
	udev.id.product = 0xfedc;
	udev.id.version = 1;

	assert(write(ufd, &udev, sizeof(udev)) >= 0);
	assert(ioctl(ufd, UI_DEV_CREATE) >= 0);

	grab();
}

void Jkb::uninit()
{
	ungrab();
}

void Jkb::handleKeyEvent(int keycode, int value)
{
	if (keycode == 16)
	{
		printf("q -> exit\n");
		running = false;
	}
	else
	{
		sendKey(keycode+1, value);
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

		n = read(efd, &ev, sizeof ev);
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

void Jkb::sendKey(int keycode, int value)
{
	// init

	struct input_event ev;

	memset(&ev, 0, sizeof(ev));

	ev.type = EV_KEY;
	ev.code = keycode;
	ev.value = value;

	// ungrab

	ungrab();

	// inject

	assert(write(ufd, &ev, sizeof(ev)) >= 0);

	// grab

	grab();
}

void Jkb::grab()
{
	assert((efd = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY)) >= 0);
	assert(ioctl(efd, EVIOCGRAB, 1) >= 0);
}

void Jkb::ungrab()
{
	assert(close(efd) >= 0);
}
